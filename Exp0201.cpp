#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>

bool is_in_set(std::set<std::string> set, std::string str){
    auto it = set.find(str);
    if(it != set.end())
        return true;
    else
        return false;
}

bool include(std::vector<std::string> unend_symbol, std::set<std::string> set){ // if set_1 /in (set)*
    for(auto it = unend_symbol.begin(); it != unend_symbol.end(); it++){
        if(!is_in_set(set, *it))
            return false;
    }
    return true;
}

std::set<std::string> OR_set (std::set<std::string> set_1, std::set<std::string> set_2){
    std::set<std::string> temp;
    for(auto it = set_1.begin(); it != set_1.end(); it++){
        temp.insert(*it);
    }
    for(auto it = set_2.begin(); it != set_2.end(); it++){
        temp.insert(*it);
    }
    return temp;
}

std::set<std::string> AND_set (std::set<std::string> set_1, std::set<std::string> set_2){
    std::set<std::string> temp;
    for(auto it = set_1.begin(); it != set_1.end(); it++){
        if(is_in_set(set_2, *it))
            temp.insert(*it);
    }
    return temp;
}

class G {
private:
    std::set<std::string> N;
    std::set<char> T;
    std::map<std::string, std::set<std::vector<std::string>>> P; //单个string表示终结符或非终结符；vector<string>表示单个生成式右侧；set表示多个生成式
    inline void input_P();
    std::string S;

    std::set<std::string> Algo_1(std::set<char> reachable_set);
    std::set<std::string> Algo_2(); //找出有用符号
    void generateReplacements(std::set<std::string> N_, std::string s, int index, std::vector<std::string>& result);
public:
    void input();
    void erase_epsilon();
    void erase_single_gener();
    void erase_unreachable();
    void output();
};

int main() {
    G g;
    g.input();
    g.erase_epsilon();
    g.erase_single_gener();
    g.erase_unreachable();
    g.output();
    return 0;
}

void G::input(){
    std::cout << "请输入非终结符个数, 与非终结符集合 N: " << std::endl;
    int n;
    std::cin >> n;
    for (int i = 0; i < n; i++) {
        std::string temp;
        std::cin >> temp;
        N.insert(temp);
    }
    std::cout << "请输入终结符个数, 与终结符集合 T: " << std::endl;
    int m;
    std::cin >> m;
    for (int i = 0; i < m; i++) {
        char temp;
        std::cin >> temp;
        T.insert(temp);
    }
    std::cout << "请输入起始符 S: " << std::endl;
    std::cin >> S;
    input_P();
}

inline void G::input_P(){
    std::cout << "请输入产生式集合 P:  // 使用#表示Epsilon空串" << std::endl;
    std::string left;
    std::vector<std::string> right;
    char c;
    c = getchar(); //读取换行符
    c = getchar();
    int state = 0; //0: 读取生成式左侧; 1: 读取生成式右侧
    while(!feof(stdin)){
        if(state == 0){
            if(c == '-'){
                state = 1;
                right.clear();
            }
            else
                left += c;
        }
        else if(state == 1){
            if(c == '\n'){
                state = 0;
                if(P.find(left) == P.end()){
                    std::set<std::vector<std::string>> temp;
                    temp.insert(right);
                    P[left] = temp;
                }
                else
                    P[left].insert(right);
                left.clear();
            }
            else{
                std::string temp(1, c);
                right.push_back(temp);
            }
        }
        c = getchar();
    }
}

std::set<std::string> G::Algo_1(std::set<char> reachable_set) {
    std::set<std::string> N0;
    std::set<std::string> N_;
    
    std::set<std::string> temp;
    for(auto it = reachable_set.begin(); it != reachable_set.end(); it++){
        temp.insert(std::string(1, *it));
    }

    for(auto it = P.begin(); it != P.end(); it++){
        for(auto itt = it->second.begin(); itt != it->second.end(); itt++){
            if(include(*itt, temp)){
                N_.insert(it->first);
                break;
            }
        }
    }
    while(N0 != N_){
        N0 = N_;
        temp = OR_set(N_, temp);
        for(auto it = P.begin(); it != P.end(); it++){
            for(auto itt = it->second.begin(); itt != it->second.end(); itt++){
                if(include(*itt, temp)){
                    N_.insert(it->first);
                    break;
                }
            }
        }
    }
    return N_;
}

std::set<std::string> G::Algo_2() {}

void G::erase_epsilon(){
    std::set<char> nullset = {'#'};
    std::set<std::string> N_ = Algo_1(nullset);//调用过算法1后已经找到的能够到达Epsilon的非终结符
    std::map<std::string, std::set<std::vector<std::string>>> P_;//需要生成的P
    std::string left;
    std::vector<std::string> right;
    //对N_中每个非终结符进行遍历，对其所有生成式进行遍历，若生成式中包含N_中的非终结符，则将其轮流替换为Epsilon，直接消去，数目先0个，再1个，直到生成式中不再包含N_中的非终结符，并把替换后的生成式加入P_
    //注意，生成式右边不能完全为Epsilon，即不能出现A->#的情况
    for(auto it = N_.begin(); it != N_.end(); it++){
        for(auto itt = P[*it].begin(); itt != P[*it].end(); itt++){
            int count = 0;
            for(auto ittt = itt->begin(); ittt != itt->end(); ittt++){
                if(N_.find(*ittt) != N_.end()){
                    count++;
                }
            }
            if(count == 0){
                if(itt->size() == 1 && (*itt)[0] == "#"){
                    continue;
                }
                else{
                    P_[*it].insert(*itt);
                }
            }
            else{
                if(itt->size() == 1)
                    P_[*it].insert(*itt);//生成式右边只有一个非终结符，且为N_中的非终结符，直接加入P_
                else{
                    std::vector<std::string> temp;
                    std::string s;
                    std::string replaced_s;
                    for(auto ittt = itt->begin(); ittt != itt->end(); ittt++){
                        s += *ittt;
                    }
                    generateReplacements(N_, s, 0, temp);
                    for(auto ittt = temp.begin(); ittt != temp.end(); ittt++){
                        for(auto itttt = ittt->begin(); itttt != ittt->end(); itttt++){
                            if(*itttt != '#')
                                replaced_s += *itttt;
                        }
                        if(replaced_s != "")
                            right.push_back(replaced_s);
                    }
                    P_[*it].insert(right);
                }
            }
        }
    }

    //若起始符能够推导出Epsilon，即N_中包含S，则需要添加一个新的起始符S1，S1->S|Epsilon
    if(N_.find(S) != N_.end()){
        left = S + "1";
        N_.insert(left);
        right.push_back(S);
        right.push_back("#");
        P_[left].insert(right);
    }
}

void G::generateReplacements(std::set<std::string> N_, std::string s, int index, std::vector<std::string>& result) {
    // 如果当前索引已经到了字符串末尾，将当前字符串加入结果集中
    if (index == s.size()) {
        result.push_back(s);
        return;
    }
    // 如果当前字符属于N_集合，则生成两个分支，一个是替换成 '#'，一个是不替换
    if (N_.find(s.substr(index, 1)) != N_.end()) {
        // 替换成 '#'
        std::string replaced_s = s;
        replaced_s[index] = '#';
        generateReplacements(N_, replaced_s, index + 1, result);
        // 不替换
        generateReplacements(N_, s, index + 1, result);
    }
    // 如果当前字符不属于，直接跳到下一个字符
    else {
        generateReplacements(N_, s, index + 1, result);
    }
}

void G::erase_single_gener(){

}

void G::erase_unreachable(){
    std::set<std::string> N1 = Algo_1(T);
    std::set<std::string> T_str;
    for(auto it = T.begin(); it != T.end(); it++){
        T_str.insert(std::string(1, *it));
    }
    this->N = N1;
    for(auto it = P.begin(); it != P.end(); it++){
        if(!is_in_set(N1, it->first)){
            if(it != P.begin()){
                it = P.erase(it);
                it--;
            }
            else{
                P.erase(it);
                it = P.begin();
            }
        }
        else{
            for(auto itt = it->second.begin(); itt != it->second.end(); itt++){
                for(auto ittt = itt->begin(); ittt != itt->end(); ittt++){
                    if(!is_in_set(OR_set(N1, T_str), *ittt)){
                        if(itt != it->second.begin()){
                            itt = it->second.erase(itt);
                            itt--;
                        }
                        else{
                            it->second.erase(itt);
                            itt = it->second.begin();
                        }
                        break;
                    }
                }
                if(itt == it->second.end()){
                    if(it != P.begin()){
                        it = P.erase(it);
                        it--;
                    }
                    else{
                        P.erase(it);
                        it = P.begin();
                    }
                    break;
                }
            }
        }
    }

    std::set<std::string> N2 = Algo_2();
}

void G::output(){
    std::cout << "上下文无关文法 G = (N, T, P, S) 为: " << std::endl;
    std::cout << "  非终结符集合\t N = { ";
    for(auto it = N.begin(); it != N.end(); it++){
        std::cout << *it;
        if(it != --N.end())
            std::cout << ", ";
    }
    std::cout << " }" << std::endl;
    std::cout << "  终结符集合\t T = { ";
    for(auto it = T.begin(); it != T.end(); it++){
        std::cout << *it;
        if(it != --T.end())
            std::cout << ", ";
    }
    std::cout << " }" << std::endl;
    std::cout << "  生成式\t P = { ";
    for(auto it = P.begin(); it != P.end(); it++){
        std::cout << it->first << " -> ";
        for(auto itt = it->second.begin(); itt != it->second.end(); itt++){
            for(auto ittt = itt->begin(); ittt != itt->end(); ittt++){
                std::cout << *ittt;
            }
            if(itt != --it->second.end())
                std::cout << " | ";
        }
        if(it != --P.end())
            std::cout << ", ";
    }
    std::cout << " }" << std::endl;
    std::cout << "  起始符\t S = " << S << std::endl;
}
