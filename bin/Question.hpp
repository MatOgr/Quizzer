#ifndef Question_hpp
#define Question_hpp

#include <string>

using namespace std;

class Question {
private:
    string content;
    string topic;
    int points;

public:
    Question();
    Question(const string cont, const string topic, const int pts);
    ~Question();

    string getContent();
    string getTopic();
    int getPoints();

    void setContent(const string);
    void setTopic(const string topic);
    void setPoints(const int pts);
};

#endif