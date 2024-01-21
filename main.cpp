/*
 * @Author: lemonmon
 * @Date: 2023-11-18 10:36:51
 * @LastEditors: lemonmon
 * @LastEditTime: 2024-01-22 00:12:03
 * @Description: SCAF示例DEMO
 */
#include "scaf.hpp"
#include <conio.h>  // getch

/*
 * 编写的应用
 */
namespace APP
{
using namespace SCAF;

class Application
{

public:
    void initApp(void)
    {
        /*
         * DEMO：设计一个应用有以下功能：
         * 主界面标题为“Main Page”，且下方显示两个选项，编号为0、1，名称为“Switch to Page1”
         * “Switch to Page2”。它们在输入对应编号时会执行对应动作。
         * 编号为0的选项在输入后会打开一个新页面，页面标题为“Page1”，且下方显示三个选项，编号为0、1、2
         * 名称为“Back to Previous Page”、“Print str1”、“Print str2”
         *  输入编号为0的选项后会返回主界面，
         *  输入编号为1的选项后会打印"hello world!"，输入任意字符后返回界面1
         *  输入编号为2的选项后会打印"Good morning!，输入任意字符后返回界面1
         * 编号为1的选项在输入后会打开一个新页面，页面标题为“Page2”，且下方显示两个选项，编号为0、5，
         * 名称为“Back to Previous Page”、“Calculate Addition”
         *  输入编号为0的选项后会返回主界面，
         *  输入编号为5的选项后会请求用户输入两个数字，并输出加法结果，输入任意字符后返回界面2。
         * 输入任何不存在的编号选项不会引起任何动作。
         * 输入esc时退出程序。
         */
        main_page = new Page("Main Page");
        page1 = new Page("Switch to Page1");
        page2 = new Page("Switch to Page2");

        // 注意，此函数new的任意Item*都应该在Application的析构函数中delete

        /* Main Page */
        main_page->addItem(new Item("Switch to Page1", main_page, 0, page1)); // 跳转到page1，空回调
        main_page->addItem(new Item("Switch to Page2", main_page, 1, page2)); // 跳转到page2，空回调

        /* PAGE 1 */
        page1->addItem(new Item("Back to Previous Page", page1, 0, main_page)); // 跳转回main_page，空回调
        page1->addItem(new Item("Print str1", page1, 1, page1,                  // 回调结束返回page1
                                [](Item const *self) -> void {
                                    cout << "hello world!\n";
                                    WaitCmd();
                                }));                           // 采用lambda函数定义逻辑，也可以写在外面
        page1->addItem(new Item("Print str2", page1, 2, page1, // 回调结束返回page1
                                [](Item const *self) -> void {
                                    cout << "Good morning!\n";
                                    WaitCmd();
                                }));

        /* PAGE 2 */
        page2->addItem(new Item("Back to Previous Page", page2, 0, main_page)); // 跳转回main_page，空回调
        page2->addItem(new Item("Calculate Addition", page2, 5, page2,
                                [](Item const *self) -> void { 
                                    float a, b;
                                    cout << "input 'a, b' to calculate a + b\n";
                                    scanf("%f, %f", &a, &b);
                                    getchar();  // 消除'\0'
                                    cout << "a + b = " << a + b << "\n"; 
                                    WaitCmd(); })); // 回调结束返回page2
    }

    Application()
    {
        initApp();
    }

    ~Application()
    {
        delete main_page;
        delete page1;
        delete page2;
        cout << "Exiting..." << endl;
    }

    void run()
    {
        char input_ch;
        Page *cur_page = main_page;
        // run forever
        while (true) {
            ClearCmdWin();
            cur_page->printPage();
            input_ch = getch();
            if (input_ch == 27) {
                break; // 输入esc时退出
            }
            Item *item_select = cur_page->getItemByIndex(input_ch - '0');
            if (!item_select) {
                continue;
            }
            // 总是先执行callback，再跳转NxtPage
            item_select->doCallback();
            cur_page = item_select->getNxtPage();
        }
    }

private:
    Page *main_page = nullptr;
    Page *page1 = nullptr;
    Page *page2 = nullptr;
};
} // namespace APP

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8); // windows下需要手动将控制台的输出编码设置为UTF-8
#endif
    APP::Application app;
    app.run();
    return 0;
}