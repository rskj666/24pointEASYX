#include <iostream>
#include <queue>
#include <algorithm>


//全局参数
const char funct[4] = { '+' ,'-','*','/' };// 定义四种运算符

enum combination_state {
    FIRST_CARD_REPEATED = -1,
    SECOND_CARD_REPEATED = -2,
    ZERO_CARD = -3,
    NORMAL = 0
}; // 定义组合状态
// 对象属性方法
class CARD {// 单张扑克牌
public:
    int num = 0;
    int position = 0; // 扑克牌在此层中的位置，0代表未定序

    CARD(const CARD& a) {// 拷贝构造函数
        this->num = a.num;
        this->position = a.position;
    }

    CARD& operator=(const CARD& a) {    // 赋值运算符重载
        this->num = a.num;
        this->position = a.position;
        return *this; // 返回对 *this 的引用
    }

    CARD(int num = 0) { // 添加卡牌构造函数
        this->num = num;
        this->position = 0;
    }

};
bool COMPARE_DOWN_CARD(const CARD& a, const CARD& b) {// 重载Card大于运算符
    return a.num > b.num;
}

const static CARD EMPTY_Card(0); // 使用异常卡牌构造函数定义空牌
const static CARD DIVE_ERROR_CARD(-1); // 使用异常卡牌构造函数定义除法错误牌

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
        CARDS[pair.SECOND_CARD.position - 1]->num = 0;
        SORT_CARDS();
        FIRST_CARD.position = 1;
        SECOND_CARD.position = 2;
        THIRD_CARD.position = 3;
        FOURTH_CARD.position = 4;
    }

    std::queue<PAIR> GET_PAIR() {//获得四张扑克牌中所有不含0的两张扑克牌的不重复的组合，因为对于4张扑克牌0牌只能被加和减法使用
        int last_card_num = -1;
        std::queue<PAIR> Q;
        ERROR_BUG(); // 这一行错误处理取决于编译器，实测VS没问题，CL必须要处理，原因不知道
        PAIR last = EMPTY_Pair;
        if (CARD_COUNT <= 1) return Q;
        for (int i = 0; i < CARD_COUNT; i++) {
            if (CARDS[i]->num == last_card_num) continue;
            last_card_num = CARDS[i]->num;
            for (int j = i + 1; j < CARD_COUNT; j++) {
                PAIR temp = PAIR(*CARDS[i], *CARDS[j]);
                if (temp == last) continue; // 避免重复
                if (temp.isempty()) continue; // 避免重复
                temp.FIRST_CARD.position = CARDS[i]->position;
                temp.SECOND_CARD.position = CARDS[j]->position;
                Q.push(temp);
                last = temp;
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
    int FOUR_FUNTION; // 运算符号
    int PAIR_FIRST_CARD_POSITION;// 第一张扑克牌的位置
    int PAIR_SECOND_CARD_POSITION;// 第二张扑克牌的位置
    int result;// 结果,如果结果为-1则代表是除法错误,如果结果为0则代表是空牌
    ONE_STEP(const int& currentstep, const POKE& poke, const int& FOUR_FUNTION_NUM, const PAIR& CHOSEN_PAIR, const CARD& RESULT) {
        // 默认构造函数
        this->step = currentstep;
        for (int i = 0; i < 4; i++) {
            this->num[i] = poke.CARDS[i]->num;
        }
        this->FOUR_FUNTION = FOUR_FUNTION_NUM;
        this->PAIR_FIRST_CARD_POSITION = CHOSEN_PAIR.FIRST_CARD.position;
        this->PAIR_SECOND_CARD_POSITION = CHOSEN_PAIR.SECOND_CARD.position;
        this->result = RESULT.num;// 如果结果为-1则代表是除法错误,如果结果为0则代表是空牌
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
// 数据库
class DATABASE {//数据库
public:
    std::vector<RESULT_STEP> result_map;//每当出现结果时，存储全局::GLOBAL_ALL_STEP和是否为24点
    DATABASE() {// 默认构造函数
        result_map.reserve(1000);
    }
    void add_RESULT(const RESULT_STEP& result_step) {// 添加结果节点
        result_map.push_back(result_step);
    }
};

static DATABASE database; // 数据库实例

class DATABASE_MANAGER {
public:
    DATABASE_MANAGER() = default;
    ~DATABASE_MANAGER() = default;
    DATABASE_MANAGER operator=(const DATABASE_MANAGER& a) = delete; // 禁止拷贝构造函数
    DATABASE_MANAGER(const DATABASE& a) = delete; // 禁止拷贝构造函数
    static void add_RESULT(const RESULT_STEP& result_step) {// 添加结果节点
        ::database.add_RESULT(result_step);
    }
};

class TWENTYFOURPOINT_GAME {// 24点游戏
private:
    TWENTYFOURPOINT_GAME() { std::cout << "WELCOME TO 24 POINT GAME" << std::endl; };
    static POKE origin_poke;
public:
    ~TWENTYFOURPOINT_GAME() { std::cout << "THANKS" << std::endl; };
    TWENTYFOURPOINT_GAME(const TWENTYFOURPOINT_GAME& a) = delete;
    static void initialize(int a, int b, int c, int d) {// 初始化函数
        CARD A(a);
        CARD B(b);
        CARD C(c);
        CARD D(d);
        TWENTYFOURPOINT_GAME::origin_poke = POKE(A, B, C, D);
    }
    // 24点游戏的深度优先搜索
    static void DFS(int step = 1, POKE& poke = TWENTYFOURPOINT_GAME::origin_poke) {
        if (poke.CARD_COUNT <= 1) {
            RESULT_STEP step_result(poke);
            DATABASE_MANAGER::add_RESULT(step_result);
            return;
        }
        std::queue<PAIR> Q;
        Q = poke.GET_PAIR();
        while (!Q.empty()) {
            PAIR temp = Q.front();
            Q.pop();
            for (int i = 0; i < 4; i++) {
                CARD result = (temp.*funcptr[i])();
                if (result.num != DIVE_ERROR_CARD.num) {
                    POKE newpoke(temp, poke.FIRST_CARD, poke.SECOND_CARD, poke.THIRD_CARD, poke.FOURTH_CARD, result);
                    ONE_STEP stepinfo(step, poke, i, temp, result);
                    ::GLOBAL_ALL_STEP.ALL_STEP_flash(stepinfo);
                    DFS(step + 1, newpoke);
                }
            }
        }
    }
    static void print_result() {
        bool FLAG = false;
        for (int i = 0; i < database.result_map.size(); i++) {
            if (database.result_map[i].is_24point) {
                FLAG = true;
                std::cout << origin_poke.FIRST_CARD.num << " " << origin_poke.SECOND_CARD.num << " " << origin_poke.THIRD_CARD.num << " " << origin_poke.FOURTH_CARD.num << "  found a solution"<<std::endl;
                for (int j = 1; j <= database.result_map[i].current_step; j++) {
                    std::cout << "Step " << j << ": ";
                    for (int k = 0; k < 4; k++) {
                        if (database.result_map[i].step_map[j].num[k] == 0) continue;
                        std::cout << database.result_map[i].step_map[j].num[k] << " ";
                    }
                    std::cout << std::endl << database.result_map[i].step_map[j].num[database.result_map[i].step_map[j].PAIR_FIRST_CARD_POSITION - 1] << " " << funct[database.result_map[i].step_map[j].FOUR_FUNTION] << " " << database.result_map[i].step_map[j].num[database.result_map[i].step_map[j].PAIR_SECOND_CARD_POSITION - 1] << " = " << database.result_map[i].step_map[j].result << std::endl;
                }
                std::cout << "-------------------------" << std::endl;
            }
        }
        if (!FLAG) std::cout << "NO RESULT" << std::endl;
    }
};
POKE TWENTYFOURPOINT_GAME::origin_poke(EMPTY_Card, EMPTY_Card, EMPTY_Card, EMPTY_Card); // 使用默认构造函数的空牌来构造空对


int main() {
    TWENTYFOURPOINT_GAME::initialize(8, 3, 2, 1);// 初始化扑克牌,四个数对应四张牌
    TWENTYFOURPOINT_GAME::DFS();
    TWENTYFOURPOINT_GAME::print_result();
    return 0;
}
