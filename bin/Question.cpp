#include "Question.hpp"

Question::Question() : 
    content("What does the foxes say?"), points(100) {};

Question::Question(const string cont, const int pts) :
    content(cont), points(pts) {};

string Question::getContent() {
    return this->content;
}

int Question::getPoints() {
    return this->points;
}

void Question::setContent(const string cont) {
    this->content = cont;
}

void Question::setPoints(const int pts) {
    this->points = pts;
}