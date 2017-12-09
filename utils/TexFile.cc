#include "TexFile.h"

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "unzip.h"

namespace {

// .zip password applied to all .tpf files
const char g_kTpfPw[] = {
    0x73, 0x2A, 0x63, 0x7D, 0x5F, 0x0A, 0xA6, 0xBD,
    0x7D, 0x65, 0x7E, 0x67, 0x61, 0x2A, 0x7F, 0x7F,
    0x74, 0x61, 0x67, 0x5B, 0x60, 0x70, 0x45, 0x74,
    0x5C, 0x22, 0x74, 0x5D, 0x6E, 0x6A, 0x73, 0x41,
    0x77, 0x6E, 0x46, 0x47, 0x77, 0x49, 0x0C, 0x4B,
    0x46, 0x6F, 0x00
};

// decrypts .tpf file to passworded zip file
unsigned TpfCrypt(void* buffer, unsigned len)
{
    unsigned int* b = (unsigned int*)buffer;
    unsigned int l = len / 4u;
    unsigned int TPF_XOR = 0x3FA43FA4u;

    for (unsigned int i = 0; i < l; i++)
        b[i] ^= TPF_XOR;
    for (unsigned int i = l * 4u; i < l * 4u + len % 4u; i++)
        ((unsigned char*)b)[i] ^= (unsigned char)TPF_XOR;
    unsigned int pos = len - 1;
    while (pos > 0u && ((char*)b)[pos])
        pos--;
    if (pos > 0u && pos < len - 1)
        len = pos + 1;
    return len;
}

char* GetTexDef(char* buf, TexFile::Texture* out)
{
    unsigned h = strtoul(buf, nullptr, 0);
    if (!h)
        return nullptr;
    out->hash = h;
    buf++;
    while (*(buf - 1) != '|')
        buf++;
    out->filename = buf;
    while (*(buf) != '\n' && *(buf) != '\r')
        buf++;
    if (*(buf) == '\r') {
        *buf = '\0';
        buf += 2;
    } else {
        *buf = '\0';
        buf += 1;
    }
    return buf;
}
}

void TexFile::LoadFromFile(char* fileName)
{
    m_lastError = 0;
    strcpy(m_fileName, fileName);

    HANDLE processHeap = GetProcessHeap();

    // read file to memory
    HANDLE fileHandle = CreateFileA(fileName, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (fileHandle == INVALID_HANDLE_VALUE) {
        m_lastError = GetLastError();
        return;
    }
    DWORD fileSize = GetFileSize(fileHandle, nullptr);
    unsigned char* compressedBase = (unsigned char*)HeapAlloc(processHeap, 0, fileSize);
    DWORD bytesRead;
    if (!ReadFile(fileHandle, compressedBase, fileSize, &bytesRead, nullptr)) {
        m_lastError = GetLastError();
        return;
    }

    // decrypt file
    HZIP zipHandle;
    if (strstr(fileName, ".tpf")) {
        m_size = TpfCrypt(compressedBase, fileSize);
        zipHandle = OpenZip(compressedBase, m_size, g_kTpfPw);
    } else {
        m_size = fileSize;
        zipHandle = OpenZip(compressedBase, m_size, nullptr);
    }

    // find and load definition file
    ZIPENTRY zEntry;
    ZRESULT zResult;
    int index;

    zResult = FindZipItem(zipHandle, "texmod.def", false, &index, &zEntry);
    if (zResult != ZR_OK) {
        m_lastError = 0x80000000 | zResult;
        return;
    }
    m_defbase = (char*)HeapAlloc(processHeap, 0, zEntry.unc_size);
    UnzipItem(zipHandle, index, m_defbase, zEntry.unc_size);

    // get amount of files + total uncompressed size
    GetZipItem(zipHandle, -1, &zEntry);
    unsigned count;
    unsigned size = zEntry.unc_size;
    m_textures = (Texture*)HeapAlloc(processHeap, HEAP_ZERO_MEMORY, zEntry.index * sizeof(Texture));

    {
        // parse definition data
        char* seeker = m_defbase;
        count = 0;
        while (seeker) {
            seeker = GetTexDef(seeker, m_textures + count);
            count++;
        }
    }

    // load all the files
    {
        for (unsigned i = 0; i < count; ++i) {
            if (!m_textures[i].filename) {
                m_texturecount = i;
                break;
                ;
            }
            zResult = FindZipItem(zipHandle, const_cast<const char*>(m_textures[i].filename), false, &index, &zEntry);
            if (zResult != ZR_OK) {
                i--;
                continue;
            }
            m_textures[i].base = HeapAlloc(processHeap, 0, zEntry.unc_size);
            m_textures[i].size = zEntry.unc_size;
            m_size += zEntry.unc_size;
            zResult = UnzipItem(zipHandle, index, m_textures[i].base, zEntry.unc_size);
            if (zResult != ZR_OK) {
                i--;
                continue;
            }
        }
    }

    CloseZip(zipHandle);
    HeapFree(processHeap, 0, compressedBase);
}

void TexFile::Release()
{
    HANDLE heap = GetProcessHeap();
    HeapFree(heap, 0, m_defbase);
    HeapFree(heap, 0, m_textures);
    HeapFree(heap, 0, m_base);
}

TexFile::Texture& TexFile::operator[](int idx)
{
    return m_textures[idx];
}
