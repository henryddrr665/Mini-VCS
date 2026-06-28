#include <iostream>
#include <fstream>
#include <string>
#include <vector>  
#include <ctime>

class Commit {
 private:
  int id;
  std::string message;
  std::string timestamp;
  std::vector<std::string> lines;

 public:
  Commit() {
    id = -1;
    message = "None";
    timestamp = "None";
  }

  void SetData(int c_id, std::string msg, std::string time_str, std::vector<std::string> content) {
    id = c_id;
    message = msg;
    timestamp = time_str;
    lines = content; 
  }

  int GetId() {
    return id;
  }
  std::string GetMessage() {
    return message;
  }
  std::string GetTimestamp() {
    return timestamp;
  }
  int GetLineCount() {
    return lines.size(); 
  }
  
  std::string GetLine(int index) {
    if ( index >= 0 && index < lines.size() ) {
      return lines.at(index);
    }
    return "";
  }

  void Print() {
    std::cout << "Commit ID: " << id << "\n";
    std::cout << "Time     : " << timestamp << "\n";
    std::cout << "Message  : " << message << "\n";
    std::cout << "Lines    : " << lines.size() << "\n";
    std::cout << "---------------------------\n";
  }
};

class VCS {
 private:
  std::vector<Commit> history;
  std::string target_file;

  std::string GetCurrentTime() {
    time_t now = time(0);
    std::string time_str = ctime(&now);
    
    std::string clean_time = "";
    for ( int i = 0; i < time_str.length(); i++ ) {
      if ( time_str[i] != '\n' ) {
        clean_time += time_str[i];
      }
    }
    return clean_time;
  }
  void SaveHistory() {
    //建立 ofstream 物件並開啟檔案並且每次存檔都覆寫
    std::ofstream file("vcs_history.txt", std::ios::trunc);
    if (!file.is_open()) {
      return;
    }
    for (int i = 0; i < history.size(); i++) {
      Commit c = history[i];
      file << "[COMMIT]\n";
      file << c.GetId() << "\n";
      file << c.GetTimestamp() << "\n";
      file << c.GetMessage() << "\n";
      // 先寫入行數，方便之後讀取
      int line_count = c.GetLineCount();
      file << line_count << "\n";
      
      // 依序寫入檔案內容
      for (int j = 0; j < line_count; j++) {
        file << c.GetLine(j) << "\n";
      }
    }
    file.close();
  }
  void LoadHistory() {
    std::ifstream file("vcs_history.txt");
    if (!file.is_open()) {
      // 檔案不存在代表是第一次執行，直接結束即可
      return; 
    }
    std::string mark;
    // 偵測到[COMMIT]標記就代表有一筆Commit資料要讀取
    while (std::getline(file, mark)) {
      if (mark == "[COMMIT]") {
        std::string id_str, time_str, msg, count_str;
        
        std::getline(file, id_str);
        std::getline(file, time_str);
        std::getline(file, msg);
        std::getline(file, count_str);
        // std::stoi將字串轉換為整數
        int c_id = std::stoi(id_str);
        int line_count = std::stoi(count_str);

        std::vector<std::string> content;
        // 根據剛才讀到的行數，用迴圈精準讀取該 Commit 的檔案內文
        for (int i = 0; i < line_count; i++) {
          std::string file_line;
          std::getline(file, file_line);
          content.push_back(file_line);
        }

        // 重新組裝成 Commit 物件放回 history 陣列
        Commit loaded_commit;
        loaded_commit.SetData(c_id, msg, time_str, content);
        history.push_back(loaded_commit);
      }
    }
    file.close();
  }



 public:
  VCS() {
    target_file = "my_code.txt"; 
    LoadHistory(); // 程式一初始化，就自動去讀取過去的commit紀錄
  }
  
  void SetTargetFile(std::string filename) {
    target_file = filename;
  }

  void PrintCurrentFile() {
    std::ifstream file(target_file);
    if ( !file.is_open() ) {
      std::cout << "File [" << target_file << "] is empty or does not exist.\n";
      return;
    }

    std::cout << "=== Content of " << target_file << " ===\n";
    std::string line;
    int count = 1;
    while ( std::getline(file, line) ) {
      std::cout << count << " | " << line << "\n";
      count++;
    }
    std::cout << "==============================\n";
    file.close();
  }

  void AppendToFile(std::string text) {
    std::ofstream file(target_file, std::ios::app); 
    if ( file.is_open() ) {
      file << text << "\n";
      std::cout << "Successfully added text to " << target_file << ".\n";
      file.close();
    } else {
      std::cout << "Error: Cannot open file to write.\n";
    }
  }

  void MakeCommit(std::string msg) {
    std::ifstream file(target_file);
    if ( !file.is_open() ) {
      std::cout << "Error: Cannot open " << target_file << " to commit.\n";
      return;
    }

    std::vector<std::string> current_lines;
    std::string line;
    
    while ( std::getline(file, line) ) {
      current_lines.push_back(line);
    }
    file.close();

    std::string time_str = GetCurrentTime();
    int current_id = history.size();
    
    Commit new_commit;
    new_commit.SetData(current_id, msg, time_str, current_lines);
    
    history.push_back(new_commit);
    std::cout << "[Success] Committed " << current_lines.size() << " lines. (Commit ID: " << current_id << ")\n";
    
    SaveHistory(); // 每次 commit 成功，就立刻同步更新到歷史檔案中
  }

  void PrintLog() {
    if ( history.size() == 0 ) {
      std::cout << "No commits yet.\n";
      return;
    }
    std::cout << "\n=== Commit History ===\n";
    for ( int i = history.size() - 1; i >= 0; i-- ) {
      history[i].Print();
    }
  }

  void Checkout(int target_id) {
    if ( target_id < 0 || target_id >= history.size() ) {
      std::cout << "Error: Invalid Commit ID.\n";
      return;
    }

    std::ofstream file(target_file); 
    if ( !file.is_open() ) {
      std::cout << "Error: Cannot write to " << target_file << ".\n";
      return;
    }

    Commit target_commit = history[target_id];
    int count = target_commit.GetLineCount();
    
    for ( int i = 0; i < count; i++ ) {
      file << target_commit.GetLine(i) << "\n";
    }
    file.close();

    std::cout << "[Success] Restored " << target_file << " to Commit ID: " << target_id << ".\n";
  }
};

int main() {
  VCS my_vcs;
  my_vcs.SetTargetFile("my_code.txt"); 

  int choice = 0;
  while ( choice != 6 ) {
    std::cout << "\n>>> Mini-VCS Menu <<<\n";
    std::cout << "1. View current file\n";
    std::cout << "2. Append text to file\n";
    std::cout << "3. Commit changes\n";
    std::cout << "4. View commit log\n";
    std::cout << "5. Checkout (Restore to old commit)\n";
    std::cout << "6. Exit\n";
    std::cout << "Choice: ";
    
    if (!(std::cin >> choice)) { // 新增防止使用者輸入英文字母導致死迴圈
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        std::cout << "Invalid input. Please enter a number.\n";
        continue;
    }
    std::cin.ignore(); // 清除換行符號

    if ( choice == 1 ) {
      my_vcs.PrintCurrentFile();
    } 
    else if ( choice == 2 ) {
      std::string input_text;
      std::cout << "Enter text to append: ";
      std::getline(std::cin, input_text);
      my_vcs.AppendToFile(input_text);
    } 
    else if ( choice == 3 ) {
      std::string msg;
      std::cout << "Enter commit message: ";
      std::getline(std::cin, msg);
      my_vcs.MakeCommit(msg);
    } 
    else if ( choice == 4 ) {
      my_vcs.PrintLog();
    } 
    else if ( choice == 5 ) {
      int id;
      std::cout << "Enter Commit ID to restore: ";
      std::cin >> id;
      std::cin.ignore();
      my_vcs.Checkout(id);
    }
  }

  return 0;
}
