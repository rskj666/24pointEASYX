#include <iostream>
#include <queue>
#include <algorithm>

char funct[4]={'+' ,'-','*','/'};
int cnt=0;
enum Ptag {
    Copy
};

class Card {
public:
    int num = 0;
    int position = 0; // 扑克牌在此层中的位置，0代表未定序

    Card(const Card &a) {
        this->num = a.num;
        this->position = a.position;
    }

    Card& operator=(const Card &a) {
        this->num = a.num;
        this->position = a.position;
        return *this; // 返回对 *this 的引用
    }

    Card(int num = 0) {
        this->num = num;
        this->position = 0;
    }

    bool operator<(const Card &a) const {
        return this->num < a.num;
    }
};
bool COMPARE_DOWN_CARD(const Card &a, const Card &b) {
    return a.num > b.num;
}


const static Card EMPTY_Card(0); // 使用默认构造函数定义空牌
const static Card DIVE_ERROR_CARD(-1); // 使用默认构造函数定义除法错误牌

class Pair {
public:
    Card FIRST_CARD;
    Card SECOND_CARD;

    Pair(const Card &a, const Card &b) {
        if (a.num > b.num) {
            FIRST_CARD = a;
            SECOND_CARD = b;
        } else {
            FIRST_CARD = b;
            SECOND_CARD = a;
        }
    }

    bool operator==(const Pair &a) const {
        return (this->FIRST_CARD.num == a.FIRST_CARD.num && this->SECOND_CARD.num == a.SECOND_CARD.num) ||
               (this->FIRST_CARD.num == a.SECOND_CARD.num && this->SECOND_CARD.num == a.FIRST_CARD.num);
    }

    bool isempty() const {
        return (this->FIRST_CARD.num == 0 && this->SECOND_CARD.num == 0);
    }
    Card PAIR_ADD() {
        return Card(FIRST_CARD.num + SECOND_CARD.num);
    }
    Card PAIR_SUB() {
        return Card(FIRST_CARD.num - SECOND_CARD.num);
    }
    Card PAIR_MUL() {
        return Card(FIRST_CARD.num * SECOND_CARD.num);
    }
    Card PAIR_DIV() {
        if (SECOND_CARD.num == 0||FIRST_CARD.num % SECOND_CARD.num != 0) return DIVE_ERROR_CARD; // 避免除以零
        return Card(FIRST_CARD.num / SECOND_CARD.num);
    }
};
Card (Pair::*funcptr[4])() = {&Pair::PAIR_ADD,&Pair::PAIR_SUB,&Pair::PAIR_MUL,&Pair::PAIR_DIV}; // 定义函数指针数组

const Pair EMPTY_Pair(EMPTY_Card, EMPTY_Card); // 使用默认构造函数的空牌来构造空对

class Poke {
public:
    Card FIRST_CARD;
    Card SECOND_CARD;
    Card THIRD_CARD;
    Card FOURTH_CARD;
    Card* CARDS[4] = {&FIRST_CARD, &SECOND_CARD, &THIRD_CARD, &FOURTH_CARD};
    int CARD_COUNT = 0;

    Poke(const Card &a = EMPTY_Card, const Card &b = EMPTY_Card, const Card &c = EMPTY_Card, const Card &d = EMPTY_Card)
        : FIRST_CARD(a), SECOND_CARD(b), THIRD_CARD(c), FOURTH_CARD(d) { // 使用初始化列表
        SORT_CARDS();
        FIRST_CARD.position = 1;
        SECOND_CARD.position = 2;
        THIRD_CARD.position = 3;
        FOURTH_CARD.position = 4;
    }
    Poke(Pair pair = EMPTY_Pair,const Card &a = EMPTY_Card, const Card &b = EMPTY_Card, const Card &c = EMPTY_Card, const Card &d = EMPTY_Card,const Card &addcard = EMPTY_Card){
        FIRST_CARD = a;
        SECOND_CARD = b;
        THIRD_CARD = c;
        FOURTH_CARD = d;
        CARDS[pair.FIRST_CARD.position-1]->num = addcard.num;
        CARDS[pair.SECOND_CARD.position-1]->num = 0;
        SORT_CARDS();
        FIRST_CARD.position = 1;
        SECOND_CARD.position = 2;
        THIRD_CARD.position = 3;
        FOURTH_CARD.position = 4;
    }
     std::queue<Pair> GET_PAIR() {
        int last_card_num=-1;
        std::queue<Pair> Q;
        Pair last = EMPTY_Pair;
        if (CARD_COUNT <= 1) return Q;
        for (int i = 0; i < CARD_COUNT; i++) {
            if (CARDS[i]->num == last_card_num) continue; // 避免重复
            last_card_num = CARDS[i]->num;
            for (int j = i + 1; j < CARD_COUNT; j++) {
                Pair temp = Pair(*CARDS[i], *CARDS[j]);
                if (temp == last) continue;
                if (temp.isempty()) continue;
                temp.FIRST_CARD.position = CARDS[i]->position;
                temp.SECOND_CARD.position = CARDS[j]->position;
                Q.push(temp);
                last = temp;
            }
        }
        return Q;
    }
    bool LAST_24() {
        if (CARD_COUNT >=2) return false;
        if (CARD_COUNT <= 1) {
            if (FIRST_CARD.num == 24) return true;
            else return false;
        }
    }
private:
    void SORT_CARDS() {
        int cnt = 0;
        Card cardpool[4] = {FIRST_CARD, SECOND_CARD, THIRD_CARD, FOURTH_CARD};
        std::sort(cardpool, cardpool + 4,::COMPARE_DOWN_CARD); // 使用 lambda 表达式作为比较函数
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

void DFS(int step,Poke &poke) {
    std::cout<<"DFS step: "<<step<<" "<<poke.FIRST_CARD.num<<" "<<poke.SECOND_CARD.num<<" "<<poke.THIRD_CARD.num<<" "<<poke.FOURTH_CARD.num<<" "<<std::endl;
    if (poke.CARD_COUNT <= 1) {
        if (poke.LAST_24()) std::cout<<"24!!"<<std::endl;
        else std::cout<<"No 24!!"<<std::endl;
        cnt++;
        return;
    }
    std::queue<Pair> Q;
    Q = poke.GET_PAIR();
    while (!Q.empty()) {
        Pair temp = Q.front();
        Q.pop();
        std::cout << "Pair: " << temp.FIRST_CARD.num <<"_"<<temp.FIRST_CARD.position<< ", " << temp.SECOND_CARD.num<<"_"<<temp.SECOND_CARD.position << std::endl;
        for (int i = 0; i < 4; i++) {
            std::cout << "Function: " << funct[i] << std::endl;
            Card result = (temp.*funcptr[i])();
            if (result.num != DIVE_ERROR_CARD.num) {
                std::cout << "Result: " << result.num << std::endl;
                Poke newpoke(temp, poke.FIRST_CARD, poke.SECOND_CARD, poke.THIRD_CARD, poke.FOURTH_CARD, result);
                DFS(step+1, newpoke);
            } else {
                std::cout << "Error in division" << std::endl;
            }
        }
    }
}
int main() {
    std::queue<Pair> Q;
    Card a(8);
    Card b(4);
    Card c(2);
    Card d(1);
    Poke poke(a, b, c, d);
    DFS(1, poke);
    std::cout << "Total: " << cnt << std::endl;
    return 0;
}
