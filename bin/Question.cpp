#include "Question.hpp"

Question::Question() : 
    content("What does the foxes say?"), 
    answers("Hau;Miau;Kszksz;Aproksymacja"),
    correct_answer(3),
    topic("life")/*, 
    points(100) */{};

Question::Question(const string cont, const string answs, const int ans_nr, const string topic) :
    content(cont), 
    answers(answs), 
    correct_answer(ans_nr), 
    topic(topic)/*, 
    points(pts) */{};

string Question::getContent() {
    return this->content;
}

string Question::getTopic() {
    return this->topic;
}

string Question::getAnswers() {
    return answers;
}

int Question::getCorrect() {
    return correct_answer;
}

bool Question::verify(const int nr) {
    return nr == correct_answer;
}

// int Question::getPoints() {
//     return this->points;
// }

void Question::setAnswers(const string answs) {
    answers = answs;
}

void Question::setCorrectAnsw(const int nr) {
    correct_answer = nr;
}

void Question::setContent(const string cont) {
    this->content = cont;
}

void Question::setTopic(const string topic) {
    this->topic = topic;
}

// void Question::setPoints(const int pts) {
//     this->points = pts;
// }
