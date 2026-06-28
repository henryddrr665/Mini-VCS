# Mini-VCS 專案

這是一個使用 C++ 開發的輕量化版本控制系統（Version Control System）。本專案模擬了 Git 的核心機制，包含檔案內容追蹤、提交歷史紀錄（Commit）、日誌查詢以及版本回溯（Checkout）功能，並具備本地端檔案系統的持久化儲存能力。

## 快速開始

1. 編譯
```bash
g++ -std=c++11 main.cpp -o mini_vcs
```
2. 執行
```bash
./mini_vcs
```
3. 功能操作說明
```
執行後會進入互動式選單，請根據提示輸入數字（1-6）：
1 查看目前檔案：印出當前目標檔案（預設為 my_code.txt）的完整內容與行號。
2 新增文字到檔案：在目標檔案的末尾追加一行新文字。
3 提交變更：儲存當前的檔案狀態。系統會要求輸入 Commit Message，並自動生成唯一的 Commit ID 與時間戳記，同時將歷史紀錄寫入 vcs_history.txt。
4 查看提交日誌：由新到舊依序列出所有過去的提交紀錄（包含 ID、時間、訊息與總行數）。
5 版本回溯 (Checkout)：輸入指定的 Commit ID，系統會將目標檔案還原至該次提交時的狀態。
6 離開：關閉程式。
```

## 模組與類別設計文件

本系統採用物件導向設計（OOD），主要由兩個核心類別相互協作，並透過本地檔案進行資料持久化。

### Commit 類別設計

負責封裝單一版本的完整資訊。

私有成員變數 (Private Members):
```c++
int id: 該次提交的唯一識別碼（從 0 開始遞增）。
std::string message: 開發者輸入的提交說明訊息。
std::string timestamp: 提交時的系統時間。
std::vector<std::string> lines: 該版本檔案逐行內容的快照陣列。
```

公有成員函式 (Public Methods)
```c++
Commit(): 建構子，初始化預設值。
void SetData(...): 設定該次提交的所有資料。
int GetId() / std::string GetMessage() / std::string GetTimestamp(): 獲取元數據。
int GetLineCount(): 獲取該版本檔案的總行數。
std::string GetLine(int index): 安全地獲取指定行號的檔案內容（包含範圍檢查）。
void Print(): 在終端機格式化印出該次 Commit 的摘要資訊。
```

### VCS 類別設計

負責檔案操作、邏輯控制與版本調度。

私有成員變數 (Private Members)
```c++
std::vector<Commit> history: 儲存所有提交紀錄的向量陣列。
std::string target_file: 當前追蹤的目標檔案名稱（預設為 my_code.txt）。
```

私有輔助函式 (Private Methods)
```c++
std::string GetCurrentTime(): 獲取當前系統時間，並自動剔除換行符號以利格式化。
void SaveHistory(): 持久化機制。將 history 內所有的 Commit 資料與檔案內容，依特定格式寫入 vcs_history.txt。
void LoadHistory(): 還原機制。程式啟動時自動讀取 vcs_history.txt，解析 [COMMIT] 標記，並重新組裝回 history 陣列。
```

公有成員函式 (Public Methods)
```c++
VCS(): 建構子，設定預設追蹤檔案，並自動呼叫 LoadHistory() 還原歷史紀錄。
void SetTargetFile(std::string filename): 切換或指定追蹤的目標檔案。
void PrintCurrentFile(): 讀取並顯示當前工作目錄下的檔案內容。
void AppendToFile(std::string text): 以附加模式（Append）將文字寫入目標檔案。
void MakeCommit(std::string msg): 讀取當前檔案，擷取快照，生成新 Commit 物件並存入歷史，最後觸發 SaveHistory()。
void PrintLog(): 逆序（最新到最舊）遍歷 history 並印出完整日誌。
void Checkout(int target_id): 驗證 ID 後，將指定 Commit 的快照內容重新覆寫回目標檔案，實現版本還原。
```

### 資料持久化格式 (vcs_history.txt)

為了確保程式關閉後紀錄不遺失，系統定義了簡單的文字儲存協定：
```
[COMMIT]
<Commit ID>
<時間戳記>
<提交訊息>
<檔案內文總行數 N>
<檔案第 1 行內容>
...
<檔案第 N 行內容>
```

這使得讀取檔案時，能透過行數 N 精準切分出檔案內容與下一筆 Commit 的標記。
