#include <iostream>
#include <queue>
#include <algorithm>
#include <mutex>
#include <thread>
#include <graphics.h>

//全局参数
const char funct[4] = { '+' ,'-','*','/' };// 定义四种运算符


enum STATE { ORIGIN, EXTRA, CARD_ERROR, EMPTY }; // 定义卡牌状态

enum combination_state {
    FIRST_CARD_REPEATED = -1,
    SECOND_CARD_REPEATED = -2,
    ZERO_CARD = -3,
    NORMAL = 0
}; // 定义组合状态

//图像参数
#define window_width 1400
#define window_height 800
#define global_fps 20
// 对象属性方法
class CARD {// 单张扑克牌
public:
    int num = 0;
    int position = 0; // 扑克牌在此层中的位置，0代表未定序
    enum STATE state = ORIGIN; // 扑克牌状态

    CARD(const CARD& a) {// 拷贝构造函数
        this->num = a.num;
        this->position = a.position;
        this->state = a.state;
    }

    CARD& operator=(const CARD& a) {    // 赋值运算符重载
        this->num = a.num;
        this->position = a.position;
        this->state = a.state;
        return *this; // 返回对 *this 的引用
    }

    CARD(int num = 0) { // 添加卡牌构造函数
        this->state = EXTRA; // 默认构造函数
        this->num = num;
        this->position = 0;
        if (this->num == 0) this->state = EMPTY; // 如果num为0，则状态为EMPTY
    }

    CARD(enum STATE state, int num = 0) {// 异常卡牌构造函数
        this->state = state;
        this->num = num;
        this->position = 0;
    }

    bool operator<(const CARD& a) const {// 重载小于运算符
        return this->num < a.num;
    }
};
bool COMPARE_DOWN_CARD(const CARD& a, const CARD& b) {// 重载Card大于运算符
    return a.num > b.num;
}


const static CARD EMPTY_Card(STATE::EMPTY, 0); // 使用异常卡牌构造函数定义空牌
const static CARD DIVE_ERROR_CARD(STATE::CARD_ERROR, -1); // 使用异常卡牌构造函数定义除法错误牌


class PAIR {    // 一对扑克牌
public:
    CARD FIRST_CARD;
    CARD SECOND_CARD;

    PAIR(const CARD& a, const CARD& b) {// 构造函数
        if (a.num > b.num) {
            FIRST_CARD = a;
            SECOND_CARD = b;
        }
        else {
            FIRST_CARD = b;
            SECOND_CARD = a;
        }
    }

    bool operator==(const PAIR& a) const {// 重载==运算符
        return (this->FIRST_CARD.num == a.FIRST_CARD.num && this->SECOND_CARD.num == a.SECOND_CARD.num) ||
            (this->FIRST_CARD.num == a.SECOND_CARD.num && this->SECOND_CARD.num == a.FIRST_CARD.num);
    }

    bool isempty() const {// 判断是否为空对
        return (this->FIRST_CARD.num == 0 && this->SECOND_CARD.num == 0);
    }
    CARD PAIR_ADD() {// 一对扑克牌的加法
        return CARD(FIRST_CARD.num + SECOND_CARD.num);
    }
    CARD PAIR_SUB() { // 一对扑克牌的减法
        int temp = FIRST_CARD.num - SECOND_CARD.num;
        CARD result(temp);
        return result;
    }
    CARD PAIR_MUL() {// 一对扑克牌的乘法
        return CARD(FIRST_CARD.num * SECOND_CARD.num);
    }
    CARD PAIR_DIV() {// 一对扑克牌的除法,如果除法有余数或者被除数为0，则返回除法错误牌
        if (SECOND_CARD.num == 0 || FIRST_CARD.num % SECOND_CARD.num != 0) return DIVE_ERROR_CARD; // 避免除以零
        return CARD(FIRST_CARD.num / SECOND_CARD.num);
    }
};

CARD(PAIR::* funcptr[4])() = { &PAIR::PAIR_ADD,&PAIR::PAIR_SUB,&PAIR::PAIR_MUL,&PAIR::PAIR_DIV }; // 定义函数指针数组指向四种运算

const PAIR EMPTY_Pair(EMPTY_Card, EMPTY_Card); // 使用默认构造函数的空牌来构造空对

class one_combination {// 单次组合节点
public:
    int First_postion;
    int Second_postion;
    int First_num;
    int Second_num;
    enum combination_state result;// -1代表第一个数重复，-2代表第二个数重复，-3代表出现0
    one_combination(int First_postion, int Second_postion, int First_num, int Second_num, enum combination_state result) {// 默认构造函数
        this->First_postion = First_postion;
        this->Second_postion = Second_postion;
        this->First_num = First_num;
        this->Second_num = Second_num;
        this->result = result;
    }
};
class ONE_STEP_COBINATION {//一次完整的组合由多个单次组合节点组成
public:
    std::vector<one_combination> one_combination_stack;
    int FIRST_CARD;
    int SECOND_CARD;
    int THIRD_CARD;
    int FOURTH_CARD;
    ONE_STEP_COBINATION(int FIRST_CARD_num, int SECOND_CARD_num, int THIRD_CARD_num, int FOURTH_CARD_num) {// 默认构造函数
        FIRST_CARD = FIRST_CARD_num;
        SECOND_CARD = SECOND_CARD_num;
        THIRD_CARD = THIRD_CARD_num;
        FOURTH_CARD = FOURTH_CARD_num;
    }
    void add(int First_postion, int Second_postion, int First_num, int Second_num, enum combination_state result) {// 添加组合
        one_combination_stack.push_back(one_combination(First_postion, Second_postion, First_num, Second_num, result));
    }

};// 记录组合

class POKE {//一组扑克牌
public:
    CARD FIRST_CARD;
    CARD SECOND_CARD;
    CARD THIRD_CARD;
    CARD FOURTH_CARD;
    CARD* CARDS[4] = { &FIRST_CARD, &SECOND_CARD, &THIRD_CARD, &FOURTH_CARD };
    int CARD_COUNT = 0;

    POKE(const CARD& a = EMPTY_Card, const CARD& b = EMPTY_Card, const CARD& c = EMPTY_Card, const CARD& d = EMPTY_Card)// 对于初始化的构造函数
        : FIRST_CARD(a), SECOND_CARD(b), THIRD_CARD(c), FOURTH_CARD(d) { // 使用初始化列表
        SORT_CARDS();
        FIRST_CARD.position = 1;
        SECOND_CARD.position = 2;
        THIRD_CARD.position = 3;
        FOURTH_CARD.position = 4;
    }
    POKE(PAIR pair = EMPTY_Pair, const CARD& a = EMPTY_Card, const CARD& b = EMPTY_Card, const CARD& c = EMPTY_Card, const CARD& d = EMPTY_Card, const CARD& addcard = EMPTY_Card) {// 对于递归构造函数
        FIRST_CARD = a;
        SECOND_CARD = b;
        THIRD_CARD = c;
        FOURTH_CARD = d;
        CARDS[pair.FIRST_CARD.position - 1]->num = addcard.num;
        CARDS[pair.FIRST_CARD.position - 1]->state = EXTRA;
        CARDS[pair.SECOND_CARD.position - 1]->num = 0;
        CARDS[pair.SECOND_CARD.position - 1]->state = EMPTY;
        SORT_CARDS();
        FIRST_CARD.position = 1;
        SECOND_CARD.position = 2;
        THIRD_CARD.position = 3;
        FOURTH_CARD.position = 4;
    }
    std::queue<PAIR> GET_PAIR(ONE_STEP_COBINATION& one_step_combination) {//获得四张扑克牌中所有不含0的两张扑克牌的不重复的组合，因为对于4张扑克牌0牌只能被加和减法使用
        int last_card_num = -1;
        std::queue<PAIR> Q;
        ERROR_BUG(); // 这一行错误处理取决于编译器，实测VS没问题，CL必须要处理，原因不知道
        PAIR last = EMPTY_Pair;
        if (CARD_COUNT <= 1) return Q;
        for (int i = 0; i < CARD_COUNT; i++) {
            if (CARDS[i]->num == last_card_num) {
                one_step_combination.add(CARDS[i]->position, CARDS[i]->position, CARDS[i]->num, CARDS[i]->num, combination_state::FIRST_CARD_REPEATED);
                continue; // 避免重复
            }
            last_card_num = CARDS[i]->num;
            for (int j = i + 1; j < CARD_COUNT; j++) {
                PAIR temp = PAIR(*CARDS[i], *CARDS[j]);
                if (temp == last) {
                    one_step_combination.add(CARDS[i]->position, CARDS[j]->position, CARDS[i]->num, CARDS[j]->num, combination_state::SECOND_CARD_REPEATED);
                    continue; // 避免重复
                }
                if (temp.isempty()) {
                    one_step_combination.add(CARDS[i]->position, CARDS[j]->position, CARDS[i]->num, CARDS[j]->num, combination_state::ZERO_CARD);
                    continue; // 避免重复
                }
                temp.FIRST_CARD.position = CARDS[i]->position;
                temp.SECOND_CARD.position = CARDS[j]->position;
                Q.push(temp);
                last = temp;
                one_step_combination.add(CARDS[i]->position, CARDS[j]->position, CARDS[i]->num, CARDS[j]->num, combination_state::NORMAL);
            }
        }
        return Q;
    }
    bool LAST_24() {// 判断是否为24点
        if (CARD_COUNT >= 2) return false;
        if (CARD_COUNT <= 1) {
            if (FIRST_CARD.num == 24) return true;
            else return false;
        }
        return false;
    }

private:
    void ERROR_BUG() {// 错误处理函数
        CARDS[0] = &FIRST_CARD;
        CARDS[1] = &SECOND_CARD;
        CARDS[2] = &THIRD_CARD;
        CARDS[3] = &FOURTH_CARD;
    }
    void SORT_CARDS() {
        int cnt = 0;
        CARD cardpool[4] = { FIRST_CARD, SECOND_CARD, THIRD_CARD, FOURTH_CARD };
        std::sort(cardpool, cardpool + 4, ::COMPARE_DOWN_CARD); // 使用 lambda 表达式作为比较函数
        FIRST_CARD = cardpool[0];
        SECOND_CARD = cardpool[1];
        THIRD_CARD = cardpool[2];
        FOURTH_CARD = cardpool[3];

        for (cnt = 0; cnt < 4; cnt++) {
            if (!cardpool[cnt].num) break;
        }
        CARD_COUNT = cnt;
    }
};
//记录栈当前状态

class ONE_STEP {
public:
    ONE_STEP() {} // 默认构造函数
    int step;//步数
    int num[4];// 一组扑克牌的四个数字
    enum STATE state[4]; // 四个数字牌的状态
    int FOUR_FUNTION; // 运算符号
    int PAIR_FIRST_CARD_POSITION;// 第一张扑克牌的位置
    int PAIR_SECOND_CARD_POSITION;// 第二张扑克牌的位置
    int result;// 结果,如果结果为-1则代表是除法错误,如果结果为0则代表是空牌
    ONE_STEP(const int& currentstep, const POKE& poke, const int& FOUR_FUNTION_NUM, const PAIR& CHOSEN_PAIR, const CARD& RESULT) {// 默认构造函数
        this->step = currentstep;
        for (int i = 0; i < 4; i++) {
            this->num[i] = poke.CARDS[i]->num;
            this->state[i] = poke.CARDS[i]->state;
        }
        this->FOUR_FUNTION = FOUR_FUNTION_NUM;
        this->PAIR_FIRST_CARD_POSITION = CHOSEN_PAIR.FIRST_CARD.position;
        this->PAIR_SECOND_CARD_POSITION = CHOSEN_PAIR.SECOND_CARD.position;
        this->result = RESULT.num;// 如果结果为-1则代表是除法错误,如果结果为0则代表是空牌
    }
    ONE_STEP(const int& currentstep, const POKE& poke, const int& FOUR_FUNTION_NUM, const PAIR& CHOSEN_PAIR, enum STATE = CARD_ERROR) {// 默认构造函数
        this->step = currentstep;
        for (int i = 0; i < 4; i++) {
            this->num[i] = poke.CARDS[i]->num;
            this->state[i] = poke.CARDS[i]->state;
        }
        this->FOUR_FUNTION = FOUR_FUNTION_NUM;
        this->PAIR_FIRST_CARD_POSITION = CHOSEN_PAIR.FIRST_CARD.position;
        this->PAIR_SECOND_CARD_POSITION = CHOSEN_PAIR.SECOND_CARD.position;
        this->result = -1;// 如果结果为-1则代表是除法错误,如果结果为0则代表是空牌
    }
};

class ALL_STEP {//全局情况记录
public:
    std::vector<ONE_STEP> step_map;
    int current_step = 0;
    ALL_STEP() {
        step_map.resize(5);
    }
    void ALL_STEP_flash(const ONE_STEP& one_step) {// 记录当前状态
        current_step = one_step.step;
        step_map[current_step] = one_step;
    }
};
static ALL_STEP GLOBAL_ALL_STEP; // 全局变量

class RESULT_STEP : public ALL_STEP {// 对结果的记录
public:
    bool is_24point = false;
    RESULT_STEP(POKE& poke, const ALL_STEP& G = ::GLOBAL_ALL_STEP) {// 默认构造函数
        is_24point = poke.LAST_24();
        this->is_24point = is_24point;
        this->current_step = G.current_step;
        this->step_map = G.step_map;
        this->step_map.resize(current_step + 1);
    }
};
class STEP_RECORD : public ALL_STEP {// 对每一步的记录::GLOBAL_ALL_STEP
public:
    STEP_RECORD(const ALL_STEP& G = ::GLOBAL_ALL_STEP) {// 默认构造函数
        this->current_step = G.current_step;
        this->step_map = G.step_map;
        this->step_map.resize(current_step + 1);
    }
};
// 数据库
class DATABASE_node {//数据库索引节点
public:
    int cnt;// 节点编号，索引从0开始
    int type;// 0代表组合节点，1代表步骤节点，2代表结果节点
    int goal_cnt;// 向对应数据库节点做映射
    DATABASE_node(int cnt, int type, int goal_cnt) {// 默认构造函数
        this->cnt = cnt;
        this->type = type;
        this->goal_cnt = goal_cnt;
    }
};// 数据库节点
class DATABASE {//数据库
public:
    int current_cnt = 0;
    std::vector<DATABASE_node> database_node_map;//存储的是线性时间表向一下各个数据库缩影的映射
    std::vector<STEP_RECORD> step_map;//存储的是每一步递归的全局::GLOBAL_ALL_STEP情况
    std::vector<ONE_STEP_COBINATION> one_step_combination;//每当需要组合算法时，记录每一个组合情况的实际调用情况
    std::vector<RESULT_STEP> result_map;//每当出现结果时，存储全局::GLOBAL_ALL_STEP和是否为24点
    DATABASE() {// 默认构造函数
        database_node_map.reserve(2000);
        step_map.reserve(1000);
        one_step_combination.reserve(1000);
        result_map.reserve(1000);
    }
    void add_ONESTEP(const STEP_RECORD& step_record) {// 添加步骤节点
        step_map.push_back(step_record);
        current_cnt++;
        DATABASE_node node(current_cnt - 1, 1, step_map.size() - 1);
        database_node_map.push_back(node);
    }
    void add_COMBINATION(const ONE_STEP_COBINATION& one_combination) {// 添加组合节点
        one_step_combination.push_back(one_combination);
        current_cnt++;
        DATABASE_node node(current_cnt - 1, 0, one_step_combination.size() - 1);
        database_node_map.push_back(node);
    }
    void add_RESULT(const RESULT_STEP& result_step) {// 添加结果节点
        result_map.push_back(result_step);
        current_cnt++;
        DATABASE_node node(current_cnt - 1, 2, result_map.size() - 1);
        database_node_map.push_back(node);
    }
};

static DATABASE database; // 数据库实例
std::mutex database_mutex;// 数据库互斥锁

class DATABASE_MANAGER {
public:
    DATABASE_MANAGER() = default;
    ~DATABASE_MANAGER() = default;
    DATABASE_MANAGER operator=(const DATABASE_MANAGER& a) = delete; // 禁止拷贝构造函数
    DATABASE_MANAGER(const DATABASE& a) = delete; // 禁止拷贝构造函数
    static void add_ONESTEP(const STEP_RECORD& step_record) {// 添加步骤节点
        std::unique_lock<std::mutex> lock(database_mutex); // 加锁
        ::database.add_ONESTEP(step_record);
        lock.unlock(); // 解锁
    }
    static void add_COMBINATION(const ONE_STEP_COBINATION& one_combination) {// 添加组合节点
        std::unique_lock<std::mutex> lock(database_mutex); // 加锁
        ::database.add_COMBINATION(one_combination);
        lock.unlock(); // 解锁
    }
    static void add_RESULT(const RESULT_STEP& result_step) {// 添加结果节点
        std::unique_lock<std::mutex> lock(database_mutex); // 加锁
        ::database.add_RESULT(result_step);
        lock.unlock(); // 解锁
    }
};

// 24点游戏的深度优先搜索
void DFS(int step, POKE& poke) {
    if (poke.CARD_COUNT <= 1) {
        RESULT_STEP step_result(poke);
        DATABASE_MANAGER::add_RESULT(step_result);
        return;
    }
    std::queue<PAIR> Q;
    ONE_STEP_COBINATION one_step_combination(poke.FIRST_CARD.num, poke.SECOND_CARD.num, poke.THIRD_CARD.num, poke.FOURTH_CARD.num);
    Q = poke.GET_PAIR(one_step_combination);
    DATABASE_MANAGER::add_COMBINATION(one_step_combination);
    while (!Q.empty()) {
        PAIR temp = Q.front();
        Q.pop();
        for (int i = 0; i < 4; i++) {
            CARD result = (temp.*funcptr[i])();
            if (result.num != DIVE_ERROR_CARD.num) {
                POKE newpoke(temp, poke.FIRST_CARD, poke.SECOND_CARD, poke.THIRD_CARD, poke.FOURTH_CARD, result);
                ONE_STEP stepinfo(step, poke, i, temp, result);
                ::GLOBAL_ALL_STEP.ALL_STEP_flash(stepinfo);
                STEP_RECORD stepin_record;
                DATABASE_MANAGER::add_ONESTEP(stepin_record);
                DFS(step + 1, newpoke);
            }
            else {
                ONE_STEP stepinfo(step, poke, i, temp, STATE::CARD_ERROR);
                ::GLOBAL_ALL_STEP.ALL_STEP_flash(stepinfo);
                STEP_RECORD stepin_record;
                DATABASE_MANAGER::add_ONESTEP(stepin_record);
            }
        }
    }
}
//图形化组件对象定义
class ITEM {
public:
    int lx, uy;
    int width, height;
    virtual ~ITEM() {} // 虚析构函数
};

class WIDGET :public ITEM {
public:
    virtual ~WIDGET() {} // 虚析构函数
    WIDGET(int x, int y, int w, int h) {
        lx = x;
        uy = y;
        width = w;
        height = h;
    }
    void update(int x, int y, int w, int h) {
        lx = x;
        uy = y;
        width = w;
        height = h;
    }
    void draw() {
        // 绘制控件
        setlinestyle(PS_SOLID, 1);
        setlinecolor(RED);
        rectangle(lx, uy, lx + width, uy + height);
    }
};
class IMAGINATION :public ITEM
{
public:
    IMAGE* thisimage;
    IMAGINATION(int x, int y, int w, int h, IMAGE& image) {
        lx = x;
        uy = y;
        width = w;
        height = h;
        thisimage = &image;
    }
    void draw() {
        putimage(lx, uy,width,height,thisimage,0,  0, SRCCOPY);
    }
};

class LABEL :public ITEM {
public:
    std::string text;
    LABEL(int x, int y, int w, int h, const std::string& t) {
        lx = x;
        uy = y;
        width = w;
        height = h;
        text = t;
    }
    void draw() {
        settextstyle(8, 0, _T("Arial"));
        settextcolor(BLACK);
        outtextxy(lx, uy, MultiByteToWideChar(CP_ACP, 0, text.c_str(), -1, NULL, 0));
    }
};
//
//图片实例
IMAGE image_firstcode;
//图形对象实例
WIDGET windows_widget(10, 10, window_width - 20, window_height - 20); // 整个窗口
WIDGET result_widget(1010, 610, 370, 170); // 结果窗口
WIDGET code_widget(1010, 20, 370, 570); // 代码窗口
IMAGINATION code_image(1020, 30, 350, 550, image_firstcode); // 代码图片
LABEL result_label(1020, 630, 350, 150, "24点123231\n"); // 结果标签

// 运行函数
// 初始化函数
POKE init(int num1, int num2, int num3, int num4) {
    loadimage(&image_firstcode, _T("CODE_FIRST.png"));
    CARD a(STATE::ORIGIN, num1);
    CARD b(STATE::ORIGIN, num2);
    CARD c(STATE::ORIGIN, num3);
    CARD d(STATE::ORIGIN, num4);
    POKE poke(a, b, c, d);
    return poke;
}

void twentyfour(POKE& poke)
{
    DFS(1, poke);
}

void graph_init()
{
    initgraph(window_width, window_height); // 新开一个画面
    setbkcolor(WHITE); // 设置背景颜色
    BeginBatchDraw(); // 开始批量绘制
}

void graph_update()
{

}

void main_show() {
    windows_widget.draw(); // 绘制整个窗口
    code_widget.draw(); // 绘制代码窗口
    result_widget.draw(); // 绘制结果窗口
    code_image.draw(); // 绘制代码图片
    result_label.draw(); // 绘制结果标签
}
void graph_show()
{
    cleardevice(); // 清屏
    main_show(); // 绘制主窗口
    FlushBatchDraw(); // 批量绘制
}
void graph()
{
    graph_init();
    while (1)
    {
        graph_update();
        graph_show();
        Sleep(1000 / global_fps);// 延时1秒
    }
}
int main() {
    POKE poke = init(1, 2, 3, 4);
    twentyfour(poke);
    graph();
    return 0;
}
