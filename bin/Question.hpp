#ifndef Question_hpp
#define Question_hpp

#include <string>

using namespace std;

class Question {
private:
    string content;
    int points;

public:
    Question();
    Question(const string cont, const int pts);
    ~Question();

    string getContent();
    int getPoints();
    void setContent(const string);
    void setPoints(const int pts);
};

#endif