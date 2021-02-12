#include "Question.hpp"

Question::Question() : 
    content("What does the foxes say?"), topic("life"), points(100) {};

Question::Question(const string cont, const string topic, const int pts) :
    content(cont), topic(topic), points(pts) {};

string Question::getContent() {
    return this->content;
}

string Question::getTopic() {
    return this->topic;
}

int Question::getPoints() {
    return this->points;
}

void Question::setContent(const string cont) {
    this->content = cont;
}

void Question::setTopic(const string topic) {
    this->topic = topic;
}

void Question::setPoints(const int pts) {
    this->points = pts;
}