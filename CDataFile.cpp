#include "CDataFile.h"

CDataFile::CDataFile()
    : m_hFile(INVALID_HANDLE_VALUE),
    m_dwRecordSize(0),
    m_ullFileSize(0),
    m_ullRecordCount(0),
    m_ullCurrentRecord(0)
{
    m_szFilePath[0] = '\0';
}

CDataFile::~CDataFile()
{
    Close();
}

BOOL CDataFile::Open(
    const char* pszFilePath,
    DWORD dwRecordSize)
{
    Close();

    if (!pszFilePath) return FALSE;
    if (dwRecordSize == 0) return FALSE;

    HANDLE hFile = CreateFile(
        pszFilePath,
        GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (hFile == INVALID_HANDLE_VALUE) {
        return FALSE;
    }

    LARGE_INTEGER liSize;

    if (!GetFileSizeEx(hFile, &liSize)) {
        CloseHandle(hFile);
        return FALSE;
    }

    m_hFile = hFile;
    m_dwRecordSize = dwRecordSize;
    m_ullFileSize = static_cast<ULONGLONG>(
        liSize.QuadPart);

    m_ullRecordCount =
        m_ullFileSize / m_dwRecordSize;

    m_ullCurrentRecord = 0;

    strcpy_s(
        m_szFilePath,
        sizeof(m_szFilePath),
        pszFilePath);

    try {
        m_RecordBuf.resize(m_dwRecordSize);
    }
    catch (...) {
        Close();
        return FALSE;
    }

    return TRUE;
}

BOOL CDataFile::ReadRecord(
    ULONGLONG ullRecordNo)
{
    if (!IsOpen()) return FALSE;
    if (ullRecordNo >= m_ullRecordCount) return FALSE;
    if (m_RecordBuf.empty()) return FALSE;

    LARGE_INTEGER liPos;

    liPos.QuadPart =
        ullRecordNo *
        static_cast<ULONGLONG>(m_dwRecordSize);

    if (!SetFilePointerEx(
        m_hFile,
        liPos,
        NULL,
        FILE_BEGIN))
    {
        return FALSE;
    }

    DWORD dwRead = 0;

    if (!ReadFile(
        m_hFile,
        m_RecordBuf.data(),
        m_dwRecordSize,
        &dwRead,
        NULL))
    {
        return FALSE;
    }

    if (dwRead != m_dwRecordSize) {
        return FALSE;
    }

    m_ullCurrentRecord = ullRecordNo;

    return TRUE;
}

void CDataFile::Close()
{
    if (m_hFile != INVALID_HANDLE_VALUE) {
        CloseHandle(m_hFile);
        m_hFile = INVALID_HANDLE_VALUE;
    }

    m_RecordBuf.clear();

    m_szFilePath[0] = '\0';

    m_dwRecordSize = 0;
    m_ullFileSize = 0;
    m_ullRecordCount = 0;
    m_ullCurrentRecord = 0;
}
