#include "header.h"

class Lexicon{
    public:
        int occupied; // how many keys currently stored in here
        int slots; // how many slots we actually have, including the ones used and not used
        int lengthOfA; // how many chars long can A be
        int* T;
        char* A;
        int indexOfA;
        int maxLoopToInsert;

        Lexicon(int slots){
            this->maxLoopToInsert = 0;
            this->occupied = 0;
            this->slots = slots;
            this->indexOfA = 0; // indicates which slot of A was the word last added to

            // for the slots of T, -1 will indicate empty
            // for the slots of T, -2 will indicate deleted
            // else, that is the value
            this->T = new int[this->slots];
            for(int i = 0; i < this->slots; i++){
                this->T[i] = -1;
            }
            this->lengthOfA = 15*this->slots; // Assuming an average word length of 14 or less (14 for word and 1 for the 0 byte separating the words)
            this->A = new char[this->lengthOfA];
        }
        ~Lexicon(){
            delete this->T;
        }

        void doubleA(){
            char* newA = new char[2*this->lengthOfA];
            for(int i = 0; i < this->lengthOfA; i++){
                newA[i] = this->A[i];
            }
            this->lengthOfA = 2*this->lengthOfA;
            delete this->A;
            this->A = newA;
            return;
        }

        void doubleT(){
            this->slots *= 2;
            delete this->T;
            this->T = new int[this->slots];
            for(int i = 0; i < this->slots; i++){
                this->T[i] = -1;
            }
            this->occupied = 0;

            char* newA = new char[this->lengthOfA];
            for(int i = 0; i<this->lengthOfA; i++){
                newA[i] = this->A[i];
            }
            int max = this->indexOfA;
            this->indexOfA = 0;
            this->A = new char[this->lengthOfA];
            
            std::string wordSoFar = "";
            for(int i = 0; i<max; i++){
                if(newA[i] != '\0'){
                    wordSoFar += newA[i];
                }
                else{
                    int len = wordSoFar.length();
                    if(wordSoFar[0] == '*'){
                        int limit = this->indexOfA + len;
                        for(; this->indexOfA<limit; this->indexOfA++){
                            this->A[this->indexOfA] = '*';
                        }
                        this->A[this->indexOfA] = '\0';
                        this->indexOfA++;
                        wordSoFar = "";
                    }
                    else{
                        this->Insert(wordSoFar);
                        wordSoFar = "";
                    }
                }
            }
        }

        bool Insert(std::string word){
            if(this->Search(word) != -1){return(false);}
            while(1 + this->indexOfA + word.length() > this->lengthOfA || this->isFull()){this->doubleA(); this->doubleT();}
            // if(this->isFull()){this->doubleT();}
            for(int i = 0; i<this->slots; i++){
                int index = hashing(i, this->sumAsciiMinusTwo(word));
                this->maxLoopToInsert++;
                if(this->T[index] < 0){
                    // free to use to store
                    // actually do the storing
                    this->T[index] = this->indexOfA;
                    for(int j=0; j<word.length(); j++){
                        this->A[this->indexOfA] = word[j];
                        this->indexOfA++;
                    }
                    this->A[this->indexOfA] = '\0';
                    this->indexOfA++;
                    this->occupied++;
                    return(true);
                }
            }
            return(false);
        }

        int sumAsciiMinusTwo(std::string word){
            int sumMinusTwo = 0;
            for(int i=0; i<word.length(); i++){
                sumMinusTwo += word[i];
            }
            sumMinusTwo -= 2;
            return(sumMinusTwo);
        }

        int hashing(int i, int k){
            return(((i*i) + k)%this->slots);
        }

        std::string wordInA(int index){
            std::string rtn = "";
            for(int i = index; i<this->lengthOfA; i++){
                if(this->A[i] == '\0'){
                    break;
                }
                else{
                    rtn += this->A[i];
                }
            }
            return(rtn);
        }

        int Search(std::string word){
            for(int i=0; i<this->maxLoopToInsert; i++){
                int index = this->hashing(i, this->sumAsciiMinusTwo(word));
                int Tindex = this->T[index];
                switch(Tindex){
                    // reached blank
                    case -1:
                        return(-1);
                    break;

                    // previously deleted entry
                    case -2:
                    break;

                    // found a spot to put it in
                    default:
                        std::string guess = this->wordInA(Tindex);
                        if(word.compare(guess) == 0){
                            return(index);
                        }
                        else if(guess.compare("") == 0 || guess[0] == '*'){
                            return(-1);
                        }
                    break;
                }
            }
            return(-1);
        }

        int Delete(std::string word){
            int loc = this->Search(word);
            if(loc == -1){
                return(-1);
            }
            this->occupied--;
            for(int i = this->T[loc]; i<this->lengthOfA; i++){
                if(A[i] != '\0'){
                    A[i] = '*';
                }
                else{
                    this->T[loc] = -2;
                    return(loc);
                }
            }
            return(-1);
        }

        std::string Print(){
            std::string rtn = " T\t\t\tA: ";
            for(int i=0; i<this->indexOfA; i++){
                if(A[i] == '\0'){
                    rtn += "\\";
                }
                else{
                    rtn += this->A[i];
                }
            }
            rtn += "\n";
            for(int i = 0; i < this->slots; i++){
                rtn += std::to_string(i) + ": ";
                rtn += this->T[i] >= 0 ? std::to_string(this->T[i]) : "";
                rtn += "\n";
            }
            return(rtn);
        }

        bool isEmpty(){
            return(this->occupied == 0);
        }

        bool isFull(){
            return(this->occupied >= this->slots);
        }
};

class HashBatch{
    public:

        std::string filename;
        Lexicon* lexLuther;

        HashBatch(std::string filename){
            this->filename = filename;
        }

        void Create(int slots){
            lexLuther = new Lexicon(slots);
            return;
        }

        int Search(std::string word){
            return(lexLuther->Search(word));
        }

        std::string Print(){
            return(lexLuther->Print());
        }

        bool Empty(){
            return(lexLuther->isEmpty());
        }

        bool Full(){
            return(lexLuther->isFull());
        }
        
        int Delete(std::string word){
            return(lexLuther->Delete(word));
        }

        bool Insert(std::string word){
            return(lexLuther->Insert(word));
        }
        
        void Batch(){
            std::string line;
            std::ifstream myfile(filename);
            if(myfile.is_open()){
                while(getline(myfile,line)){

                    std::string firstWord = line.substr(0, line.find(" "));

                    // Command 10 is Insert, Command 11 is Deletion, Command 12 is Search. Command 13 is Print, Command 14 is Create. Command 15 is Comment

                    // Insert
                    if(firstWord.compare("10") == 0){
                        std::string secondWord = line.substr(firstWord.length() + 1, line.length() - 1);
                        bool inserted = Insert(secondWord);
                        if(!inserted){
                            // Not Inserted
                            int loc = Search(secondWord);
                            std::cout << secondWord + " ";
                            if(loc == -1){
                                std::cout << "\toverflow and cannot insert";
                            }
                            else{
                                std::cout << "\tfound at slot " + std::to_string(loc);
                            }
                            std::cout << "\n";
                        }
                    }

                    // Delete
                    else if(firstWord.compare("11") == 0){
                        std::string secondWord = line.substr(firstWord.length() + 1, line.length() - 1);
                        int deletedIndex = Delete(secondWord);
                        std::cout << secondWord + " ";
                        if(deletedIndex == -1){
                            std::cout << "\tnot found";
                        }
                        else{
                            std::cout << "\tdeleted from slot " + std::to_string(deletedIndex);
                        }
                        std::cout << "\n";
                    }

                    // Search
                    else if(firstWord.compare("12") == 0){
                        std::string secondWord = line.substr(firstWord.length() + 1, line.length() - 1);
                        int loc = Search(secondWord);
                        std::cout << secondWord + " ";
                        if(loc == -1){
                            std::cout << "\tnot found";
                        }
                        else{
                            std::cout << "\tfound at slot " + std::to_string(loc);
                        }
                        std::cout << "\n";
                    }

                    // Print
                    else if(firstWord.compare("13") == 0){
                        std::cout << Print();
                    }

                    // Create
                    else if(firstWord.compare("14") == 0){
                        std::string secondWord = line.substr(firstWord.length() + 1, line.length() - 1);
                        int slots = stoi(secondWord);
                        Create(slots);
                    }

                    // Comment
                    else if(firstWord.compare("15") == 0){
                        continue;
                    }
                }
                myfile.close();
            }
            return;
        }
};

int main(int argc, char* argv[]){
    std::string filename = argv[1];
    HashBatch* batman = new HashBatch(filename);
    batman->Batch();
    delete batman;
    return(0);
}
