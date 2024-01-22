/*
 * @Author: lemonmon
 * @Date: 2024-01-21 19:38:29
 * @LastEditors: lemonmon
 * @LastEditTime: 2024-01-22 13:29:46
 * @Description: 一个构造命令行应用的简单框架。
 * 本框架只考虑页面Page上有控件Item，以及Page和Page之间通过
 * Item选项切换，并运行选择Item后执行特定回调函数。
 * 详细用例查看main.cpp的Application类实现。
 */
#pragma once

#include <iomanip>
#include <iostream>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <conio.h> // getch
#include <windows.h>
#else
char getch(){
    char ch;
    system("stty -echo");
    system("stty -icanon");
    ch = getchar();
    system("stty echo");
    system("stty icanon");
    return ch;
}
#endif

// #define delnull(p)   \
//     do {             \
//         delete p;    \
//         p = nullptr; \
//     } while (0);

void clearCmdWin(void)
{
#ifdef _WIN32
    system("cls"); // windows cmd
#else
    system("clear"); // linux cmd
#endif
}

void waitCmd(void)
{
    std::cout << "Press Enter to Continue...";
    getchar();
}

void clearInBuffer(void)
{
    while (getchar() != '\n') {
    }
}

namespace SCAF
{
class Page;
class Item;
class Controller;

using namespace std;
constexpr unsigned int MAX_STR_LEN = 65536;

/*
 * 页面中基本操作单元Item
 * 仅支持Item的Index为单字符。
 */
class Item
{
public:
    Item() {}
    Item(const char *name_,
         Page *const loc_page_,
         const char index_,
         Page *const nxt_page_ = nullptr,
         void (*const fcallback_)(Item const *self) = nullptr) // 回调函数传入自身地址，以便回调中调用Item的相关信息
        : name(name_), loc_page(loc_page_), index(index_), nxt_page(nxt_page_), fcallback(fcallback_)
    {
    }
    ~Item() {}

    const char *getName(void)
    {
        return name;
    }

    void setName(const char *name_)
    {
        name = name_;
    }

    Page *getLocPage(void)
    {
        return loc_page;
    }

    int getIndex(void)
    {
        return index;
    }

    Page *getNxtPage(void)
    {
        return nxt_page;
    }

    void doCallback(void)
    {
        // nullptr不做任何事情
        if (!fcallback) {
            return;
        }
        fcallback(this);
    }

protected:
    const char *name = "\0";
    Page *loc_page = nullptr;
    char index = 0;
    Page *nxt_page = nullptr;
    void (*fcallback)(Item const *self) = nullptr; // 回调函数地址，此处假设回调函数没有入参
};

/*
 * 承载Item的页面
 * 注意目前打印相关只支持英文对齐，中文对齐存在问题。
 */
constexpr unsigned int MAX_ITEM_ONE_PAGE = 128;
constexpr unsigned int LINES_BND_WDITH = 80;
class Page
{
public:
    Page() {}
    Page(const char *title_) : title(title_) {}
    ~Page()
    {
        // page析构时回收其上所有item
        for (int i = 0; i < items_num; ++i) {
            delete items[i];
            items[i] = nullptr;
        }
    }

    /*
     * 此函数add的Item会在该page析构时释放。
     */
    bool addItem(Item *const item)
    {
        if (items_num >= MAX_ITEM_ONE_PAGE) {
            cout << "当前页面中Item数量大于最大值，请重新设置最大容量。" << endl;
            return false;
        }
        items[items_num] = item;
        items_num += 1;
        return true;
    }

    Item *getItemByIndex(const int index)
    {
        for (int i = 0; i < items_num; ++i) {
            if (items[i]->getIndex() == index) {
                return items[i];
            }
        }
        return nullptr;
    }

    void printPage(void)
    {
        int s = 0;
        bool is_print_over = false;
        while (!is_print_over) {
            switch (s) {
            case 0: {
                printTitle();
            } break;
            case 1: {
                printItem();
            } break;
            case 2: {
                printEndLine();
            } break;
            default: {
                is_print_over = true;
            } break;
            }
            s += 1;
        }
    }

    void printTitle(void)
    {
        printWholeLine(LINES_BND_WDITH, '*');
        cout << endl;
        printWrappedStrMiddle(LINES_BND_WDITH, '*', title);
        cout << endl;
        printWholeLine(LINES_BND_WDITH, '*');
        cout << endl;
    }

    void printItem(void)
    {
        for (int i = 0; i < items_num; ++i) {
            printWrappedNumberedStrLeft(items[i]->getIndex(),
                                        LINES_BND_WDITH,
                                        '*',
                                        items[i]->getName());
            cout << endl;
        }
    }

    void printEndLine(void)
    {
        printWholeLine(LINES_BND_WDITH, '*');
        cout << endl;
    }

private:
    const char *title = "\0";
    Item *items[MAX_ITEM_ONE_PAGE] = {nullptr};
    int items_num = 0;

    static inline void printWholeLine(const unsigned int len, const char c)
    {
        for (int i = 0; i < len; ++i) {
            cout << c;
        }
    }

    static inline void printWrappedNumberedStrLeft(
        const char num,
        const unsigned int len,
        const char wrapper,
        const char *str)
    {
        // 注意本函数的num打印为char
        size_t str_len = strlen(str);
        cout << wrapper;
        cout << num << ": " << str;
        size_t valid_str_len = str_len + 5;
        if (len >= valid_str_len) {
            printWholeLine(len - valid_str_len, ' ');
            cout << wrapper;
        } else {
            char tmp[MAX_STR_LEN];
            strncpy(tmp, str, str_len);
            tmp[len - 2] = '\0';
            cout << tmp;
            cout << wrapper;
        }
    }

    static inline void printWrappedStrMiddle(
        const unsigned int len,
        const char wrapper,
        const char *str)
    {
        /*
         * 如果给定的长度足以放进整个str，则放在中间位置
         * 否则为了格式整齐截断str
         */
        size_t str_len = strlen(str);
        int start_pos = (len - 2 - str_len) / 2; // 除去头尾的wrapper
        cout << wrapper;
        if (start_pos >= 0) {
            int width = str_len + start_pos + 1;
            printWholeLine(start_pos, ' ');
            cout << str;
            printWholeLine(len - width - 1, ' ');
            cout << wrapper;
        } else {
            char tmp[MAX_STR_LEN];
            strncpy(tmp, str, str_len);
            tmp[len - 2] = '\0';
            cout << tmp;
            cout << wrapper;
        }
    }
};

}; // namespace SCAF