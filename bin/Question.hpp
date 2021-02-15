#ifndef Question_hpp
#define Question_hpp

#include <string>

using namespace std;

class Question {
private:
    string content;
    string answers;
    int correct_answer;
    string topic;
    int points;

public:
    Question();
    Question(const string cont, const string answs, const int ans_nr, const string topic, const int pts);
    ~Question();

    string getContent();
    string getTopic();
    string getAnswers();
    int getPoints();
    bool verify(const int nr);

    void setAnswers(const string ans);
    void setContent(const string cont);
    void setTopic(const string topic);
    void setPoints(const int pts);
};

#endif