//输入为目录名，输出为目录中的所有文件

#include <vector>
#include <string>

using std::vector;
using std::string;

class DirScanner{
private:
    //用于存储目录中所有的文件绝对路径
    vector<string> _files;

public:
    //用于返回_files
    vector<string> &getFiles();

    //用于传递所有文件
    void traverse(string dir);

    //用于清空_files
    void clean();
};