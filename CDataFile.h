#pragma once

#include <windows.h>
#include <vector>

class CDataFile
{
public:
    CDataFile();
    ~CDataFile();

    BOOL Open(const char* pszFilePath, DWORD dwRecordSize);
    void Close();

    BOOL ReadRecord(ULONGLONG ullRecordNo);

    BOOL IsOpen() const {
        return (m_hFile != INVALID_HANDLE_VALUE);
    }

    const BYTE* GetRecordData() const {
        return m_RecordBuf.empty() ? NULL : m_RecordBuf.data();
    }

    DWORD GetRecordSize() const {
        return m_dwRecordSize;
    }

    ULONGLONG GetRecordCount() const {
        return m_ullRecordCount;
    }

    ULONGLONG GetCurrentRecord() const {
        return m_ullCurrentRecord;
    }

    const char* GetFilePath() const {
        return m_szFilePath;
    }

    ULONGLONG GetFileSize() const {
        return m_ullFileSize;
    }

    ULONGLONG GetRemainSize() const {
        if (m_dwRecordSize == 0) {
            return 0;
        }

        return m_ullFileSize % m_dwRecordSize;
    }

private:
    HANDLE m_hFile;

    char m_szFilePath[MAX_PATH];

    DWORD m_dwRecordSize;

    ULONGLONG m_ullFileSize;
    ULONGLONG m_ullRecordCount;
    ULONGLONG m_ullCurrentRecord;

    std::vector<BYTE> m_RecordBuf;
};
