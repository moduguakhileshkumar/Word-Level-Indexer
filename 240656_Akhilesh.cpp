#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cctype>
#include <algorithm>
#include <chrono>

using namespace std;

template<typename T>
void printContainer(const T& container){
    for(const auto& p : container){
        cout << p.first << " " << p.second << endl;
    }
}

class FileReader {
private:
    ifstream file;
    size_t bufferSize;
    vector<char> buffer;

public:
    FileReader(const string& filePath, size_t bufferKB) {
        if (bufferKB < 256 || bufferKB > 1024)
            throw invalid_argument("Buffer size must be 256–1024 KB.");

        bufferSize = bufferKB * 1024;
        buffer.resize(bufferSize);

        file.open(filePath, ios::binary);

        if (!file.is_open())
            throw runtime_error("Failed to open file.");
    }

    bool readChunk(string& output) {
        file.read(buffer.data(), bufferSize);
        streamsize bytesRead = file.gcount();

        if (bytesRead <= 0)
            return false;

        output.assign(buffer.data(), bytesRead);
        return true;
    }

    ~FileReader() {
        if (file.is_open())
            file.close();
    }
};

unordered_map<string, unordered_map<string,int>> versions;

void tokenizeAndCount(const string& chunk, unordered_map<string,int>& freq) {
    string token;
    for (char ch : chunk) {
        if (isalnum((unsigned char)ch))
            token += ch;
        else {
            if (!token.empty()) {
                freq[token]++;
                token.clear();
            }
        }
    }
    if (!token.empty())
        freq[token]++;
}

void buildVersion(const string& filePath,const string& versionName,int bufferSize){

    FileReader reader(filePath, bufferSize);

    unordered_map<string,int> freq;
    freq.reserve(2000000);

    string chunk;
    string leftover = "";

    while (reader.readChunk(chunk)) {

        for (char &c : chunk)
            c = tolower((unsigned char)c);

        chunk = leftover + chunk;
        leftover = "";

        int last = chunk.size() - 1;

        while (last >= 0 && isalnum((unsigned char)chunk[last]))
            last--;

        if (last != chunk.size() - 1) {
            leftover = chunk.substr(last + 1);
            chunk = chunk.substr(0, last + 1);
        }

        tokenizeAndCount(chunk, freq);
    }

    if (!leftover.empty())
        freq[leftover]++;

    versions[versionName] = move(freq);
}

void buildVersion(const string& filePath,const string& versionName,int bufferSize,bool verbose){
    buildVersion(filePath,versionName,bufferSize);
    if(verbose){
        cout<<"Version "<<versionName<<" indexed successfully"<<endl;
    }
}

class Query{
public:
    virtual void execute() = 0;
    virtual ~Query(){}
};

class WordQuery : public Query {
private:
    string version;
    string word;

public:
    WordQuery(string v,string w){
        version=v;
        word=w;
    }

    void execute() override{
        if(!versions.count(version)){
            cout<<"Version not found\n";
            return;
        }

        auto &freq = versions[version];

        if(freq.count(word)){
            cout<<"Version : "<<version<<endl;
            cout<<"Count : "<<freq[word]<<endl;
        }
        else{
            cout<<"Word not found\n";
        }
    }
};

class TopKQuery : public Query {
private:
    string version;
    int k;

public:
    TopKQuery(string v,int k_val){
        version=v;
        k=k_val;
    }

    void execute() override{

        if(!versions.count(version)){
            cout<<"Version not found\n";
            return;
        }

        auto &freq = versions[version];

        vector<pair<string,int>> vec(freq.begin(),freq.end());

        sort(vec.begin(),vec.end(),
             [](auto &a,auto &b){
                 return a.second>b.second;
             });

        cout<<"Top-"<<k<<" words in version: "<<version<<endl;

        for(int i=0;i<k && i<vec.size();i++)
            cout<<vec[i].first<<" "<<vec[i].second<<endl;
    }
};

class DiffQuery : public Query {
private:
    string v1;
    string v2;
    string word;

public:
    DiffQuery(string a,string b,string w){
        v1=a;
        v2=b;
        word=w;
    }

    void execute() override{

        if(!versions.count(v1)||!versions.count(v2)){
            cout<<"One or both versions not found\n";
            return;
        }

        int c1=0;
        int c2=0;

        if(versions[v1].count(word))
            c1=versions[v1][word];

        if(versions[v2].count(word))
            c2=versions[v2][word];

        cout<<"Difference (v2-v1): "<<(c2-c1)<<endl;
    }
};

int main(int argc,char* argv[]){

    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    auto programStart = chrono::high_resolution_clock::now();

    vector<pair<string,string>> pendingVersions;

    string filePath,versionName;
    string queryType;
    string queryWord;
    string version1,version2;

    int buffer_size=0;
    int k=0;

    for(int i=1;i<argc;i++){

        string arg=argv[i];

        if(arg=="--file" && i+1<argc){

            filePath=argv[++i];

            if(i+2<argc && string(argv[i+1])=="--version"){

                versionName=argv[i+2];
                i+=2;

                pendingVersions.push_back({filePath,versionName});
            }
        }

        else if(arg=="--buffer" && i+1<argc)
            buffer_size=stoi(argv[++i]);

        else if(arg=="--query" && i+1<argc)
            queryType=argv[++i];

        else if(arg=="--word" && i+1<argc)
            queryWord=argv[++i];

        else if(arg=="--top" && i+1<argc)
            k=stoi(argv[++i]);

        else if(arg=="--file1" && i+1<argc){

            filePath=argv[++i];

            if(i+2<argc && string(argv[i+1])=="--version1"){

                version1=argv[i+2];
                i+=2;

                pendingVersions.push_back({filePath,version1});
            }
        }

        else if(arg=="--file2" && i+1<argc){

            filePath=argv[++i];

            if(i+2<argc && string(argv[i+1])=="--version2"){

                version2=argv[i+2];
                i+=2;

                pendingVersions.push_back({filePath,version2});
            }
        }
    }

    for(auto &p:pendingVersions)
        buildVersion(p.first,p.second,buffer_size);

    try{

        for(char& c:queryWord)
            c=tolower((unsigned char)c);

        Query* q=nullptr;

        if(queryType=="word")
            q=new WordQuery(versionName,queryWord);

        else if(queryType=="top")
            q=new TopKQuery(versionName,k);

        else if(queryType=="diff")
            q=new DiffQuery(version1,version2,queryWord);

        if(q){
            q->execute();
            delete q;
        }

    }
    catch(const exception& e){
        cerr<<"Error: "<<e.what()<<endl;
    }

    cout<<"Buffer Size (KB): "<<buffer_size<<endl;

    auto programEnd = chrono::high_resolution_clock::now();
    chrono::duration<double> totalDuration = programEnd-programStart;

    cout<<"Execution time (s): "<<totalDuration.count()<<endl;

    return 0;
}