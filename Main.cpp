#include<iostream>
#include<map>
#include<forward_list>
#include<string.h>
#include<unistd.h>
#include<fstream>
#include<sstream>
#include<bits/stdc++.h>
#include<sys/stat.h>

#ifdef _WIN32
#include<windows.h>
#endif

#ifdef linux
#include<arpa/inet.h>
#include<sys/socket.h>
#endif

using namespace std;


// Chirayu (The Creator of HTTP Web Server)

class BroUtilities
{
    private:
    BroUtilities() {}

    public:
    static void loadMIMETypes(map<string, string> &mimeTypesMap)
    {
        ifstream file("data-for-web-server/mime.types");
        if(!file.is_open())
        {
            cerr<<"Failed to open mime.types file"<<endl;
            return;
        }

        string line;
        while(getline(file, line))
        {
            // Skip empty lines and comments
            if (line.empty() || line[0] == '#')
                continue;

            // Finding the first tab separating mime type from extensions
            size_t tabPos = line.find('\t');
            if(tabPos == string::npos) continue; // No tab found — malformed line, skip it safely

            // Extracting mime type (everything before the first tab)
            string mimeType = line.substr(0, tabPos);

            // Trimimming trailing whitespace from mimeType
            while(!mimeType.empty() && (mimeType.back() == ' ' || mimeType.back() == '\r'))
                mimeType.pop_back();

            if(mimeType.empty()) continue;

            // Extracting extensions portion (everything after the tab(s))
            size_t extStart = line.find_first_not_of('\t', tabPos);
            if(extStart == string::npos)
                continue; // If no extensions on this line, skip

            string extPart = line.substr(extStart);

            // Trimming trailing \r
            while(!extPart.empty() && (extPart.back() == '\r' || extPart.back() == '\n'))
                extPart.pop_back();

            if (extPart.empty())
                continue;

            // Splitting extensions by space and tab, mapping each to the mime type
            istringstream extStream(extPart);
            string extension;
            while(extStream >> extension) // >> skips all whitespace automatically
            {
                if(!extension.empty())
                {
                    mimeTypesMap.insert({extension, mimeType});
                    // cout<<extension<<"   ,   "<<mimeType<<endl;
                }
            }
        }

        file.close();
    }
};


class FileSystemUtility
{
    private:
    FileSystemUtility(){};

    public:
    static bool fileExists(const char *path)
    {
        int x;
        struct stat s;
        x = stat(path, &s);
        if(x != 0) return false;    // file does not exist
        if(s.st_mode & S_IFDIR) return false;   // if the value is non zero means it is a directory not a file
        return true;
    }

    static bool directoryExists(const char *path)
    {
        int x;
        struct stat s;
        x = stat(path, &s);
        if(x != 0) return false;    // directory does not exist
        return (s.st_mode & S_IFDIR);   // if the value is non zero means it is a directory not a file
    }

    static string getFileExtension(const char *path)
    {
        int x;
        x = strlen(path) - 1;
        while(x >= 0 && path[x] != '.') x--;
        if(x == -1 || path[x] != '.') return string("");
        return string(path + (x + 1));
    }
};

class StringUtility
{
    private:
    StringUtility();
    public:
    static void toLowerCase(char *str)
    {
        if(str==NULL) return;
        for(;*str;str++) if(*str>=65 && *str<=90) *str+=32;
    }
};

class HttpErrorStatusUtility
{
    private:
    HttpErrorStatusUtility(){}
    public:
    static void sendBadRequestError(int clientSocketDescriptor)
    {
        // will do later on
    }
    static void sendHttpVersionNotSupportedError(int clientSocketDescriptor, char *httpVersion)
    {
        // will do later on
    }
    static void sendNotFoundError(int clientSocketDescriptor, char *requestURI)
    {
        cout<<"NOT FOUND"<<endl;
        // will optimize this code later on
        char content[1000];
        char header[200];
        char response[1200];
        sprintf(content,"<!doctype html><html lang='en'><head><meta charset='utf-8'><title>404 Error</title></head><body>Requested Resource [%s] Not Found</body>", requestURI);
        int contentLength = strlen(content);
        sprintf(header,"HTTP/1.1 404 Not Found\r\nContent-Type:text/html\nContent-Length:%d\nConnection: close\r\n\r\n", contentLength);
        strcpy(response,header);
        strcat(response,content);
        send(clientSocketDescriptor, response, strlen(response),0);
    }
    static void sendMethodNotAllowedError(int clientSocketDescriptor, char *method, char *requestURI)
    {
        // will do later on
    }
};

class Validator
{
    private:

    Validator(){} // Constructor in private so that this class cannot be instanciated

    public:

    static bool isValidMIMEType(string &mimeType)
    {
        return true;
    }

    static bool isValidPath(string &path)
    {
        return FileSystemUtility::directoryExists(path.c_str());
    }

    static bool isValidURLFormat(string &url)
    {
        return true;
    }
};

class Error
{
    private:

    string error;

    public:

    Error(string error)
    {
        this->error = error;
    }

    bool hasError()
    {
        return (this->error.length() > 0);
    }

    string getError()
    {
        return this->error;
    }
};

class Request
{
    private:
    map<string, string> dataMap;
    char *method;
    char *requestURI;
    char *httpVersion;
    Request(char *method, char *requestURI, char *httpVersion, char *dataInRequest)
    {
        this->method = method;
        this->requestURI = requestURI;
        this->httpVersion = httpVersion;
        if(dataInRequest != NULL && strcmp(this->method, "get") == 0)
        {
            createDataMap(dataInRequest, dataMap);
        }
    }

    void createDataMap(char *str, map<string,string> &dataMap)
    {
        char *ptr1, *ptr2;
        ptr1 = str;
        ptr2 = str;
        while(true)
        {
            while(*ptr2 != '\0' && *ptr2 != '=') ptr2++;
            if(*ptr2 == '\0') return;
            *ptr2 = '\0';
            string key = string(ptr1);
            ptr1 = ptr2 + 1;
            ptr2 = ptr1;
            while(*ptr2 != '\0' && *ptr2 != '&') ptr2++;
            if(*ptr2 == '\0')
            {
                dataMap.insert(pair<string,string>(key, string(ptr1)));
                break;
            }
            else
            {
                *ptr2 = '\0';
                dataMap.insert(pair<string,string>(key,string(ptr1)));
                ptr1 = ptr2 + 1;
                ptr2 = ptr1;
            }
        }  // end of infinite loop
    }

    public:
    string operator[](string key)
    {
        auto iterator = dataMap.find(key);
        if(iterator == dataMap.end())
        {
            return string("");
        }
        return iterator->second;
    }

    friend class Bro;
};

class Response
{
    private:

    string contentType;
    forward_list<string> content;
    forward_list<string>::iterator contentIterator;
    unsigned long contentLength;

    public:

    Response()
    {
        this->contentLength = 0;
        this->contentIterator = this->content.before_begin();
    }

    ~Response()
    {

    }

    void setContentType(string contentType)
    {
        if(Validator::isValidMIMEType(contentType))
        {
            this->contentType = contentType;
        }
        else
        {

        }
    }

    Response & operator<<(string content)
    {
        this->contentLength += content.length();
        this->contentIterator = this->content.insert_after(this->contentIterator, content);
        return *this;
    }
    friend class HttpResponseUtility;
};

class HttpResponseUtility
{
    private:
    HttpResponseUtility(){}
    public:
    static void sendResponse(int clientSocketDescriptor, Response &response)
    {
        char header[200];
        int contentLength=response.contentLength;
        sprintf(header,"HTTP/1.1 200 OK\r\nContent-Type:text/html\nContent-Length:%d\nConnection: close\r\n\r\n", contentLength);
        send(clientSocketDescriptor,header,strlen(header),0);
        auto contentIterator = response.content.begin();
        while(contentIterator != response.content.end())
        {
            string str = *contentIterator;
            send(clientSocketDescriptor,str.c_str(),str.length(),0);
            ++contentIterator;
        }
    }
};

enum __request_method__{__GET__, __POST__, __PUT__, __DELETE__, __CONNECT__, __HEAD__, __OPTIONS__, __TRACE__};

typedef struct __url__mapping
{
__request_method__ requestMethod;
void (*mappedFunction)(Request &, Response &);
}URLMapping;

class Bro
{
    private:

    string staticResourcesFolder;
    map<string, URLMapping> urlMappings;
    map<string, string> mimeTypes;

    public:

    Bro()
    {
	// cout<<"Default Constructure got invoked"<<endl;
        BroUtilities::loadMIMETypes(mimeTypes);
        // cout<<"MIME Types size : "<<mimeTypes.size()<<endl;
        if(mimeTypes.size() == 0) throw string("data-for-web-server folder has been tampered with");
    }

    ~Bro()
    {

    }

    void setStaticResourcesFolder(string staticResourcesFolder)
    {
        if(Validator::isValidPath(staticResourcesFolder))
        {
            this->staticResourcesFolder = staticResourcesFolder;
        }
        else
        {
            string exception = string("Invalid static resource folder path : ") + staticResourcesFolder;
            throw exception;
        }
    }

    bool serveStaticResource(int clientSocketDescriptor, const char *requestURI)
    {
        if(this->staticResourcesFolder.length() == 0) return false;
        if(!FileSystemUtility::directoryExists(this->staticResourcesFolder.c_str())) return false;
        string resourcePath = this->staticResourcesFolder + string(requestURI);
        cout<<"Static resource path is : "<<resourcePath<<endl;
        if(!FileSystemUtility::fileExists(resourcePath.c_str())) return false;

        FILE *file = fopen(resourcePath.c_str(), "rb");
        if(file == NULL) return false;
        long fileSize;
        fseek(file, 0, SEEK_END);
        fileSize = ftell(file);
        if(fileSize == 0)
        {
            fclose(file);
            return false;
        }
        rewind(file); // to move the internal file pointer to the start of the file
        string extension, mimeType;
        extension = FileSystemUtility::getFileExtension(resourcePath.c_str());
        if(extension.length() > 0)
        {
            transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
            auto mimeTypesIterator = mimeTypes.find(extension);
            if(mimeTypesIterator != mimeTypes.end())
            {
                mimeType = mimeTypesIterator->second;
            }
            else
            {
                mimeType = string("text/html");
            }
        }
        else
        {
            mimeType = string("text/html");
        }
        cout<<resourcePath<<"   ,   "<<extension<<"  ,   "<<mimeType<<endl;

        char header[200];
        sprintf(header, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: %ld\r\nConnection: close\r\n\r\n", fileSize);
        send(clientSocketDescriptor, header, strlen(header), 0);
        long bytesLeftToRead;
        int bytesToRead;
        char buffer[4096];
        bytesToRead = 4096;
        bytesLeftToRead = fileSize;
        while(bytesLeftToRead > 0)
        {
            if(bytesLeftToRead < bytesToRead) bytesToRead = bytesLeftToRead;
            fread(buffer, bytesToRead, 1, file);
            if(feof(file)) break; // this won't happen in our case but adding this, just to be on the safer side
            send(clientSocketDescriptor, buffer, bytesToRead, 0);
            bytesLeftToRead -= bytesToRead;
        }
        fclose(file);
        return true;
    }

    void get(string url, void (*callBack)(Request &, Response &))
    {
        if(Validator::isValidURLFormat(url))
        {
            urlMappings.insert(pair<string, URLMapping>(url, {__GET__, callBack}));
        }
    }

    void post(string url, void (*callBack)(Request &, Response &))
    {
        if(Validator::isValidURLFormat(url))
        {
            urlMappings.insert(pair<string, URLMapping>(url, {__POST__, callBack}));
        }
    }

    void listen(int portNumber, void (*callBack)(Error &))
    {
        #ifdef _WIN32
        WSADATA wsaData;
        WORD ver;
        ver = MAKEWORD(1, 1);
        WSAStartup(ver, &wsaData);
        #endif

        int serverSocketDescriptor;
        char requestBuffer[4097]; // one extra byte for \0
        int requestLength;
        int x;
        serverSocketDescriptor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if(serverSocketDescriptor < 0)
        {
            #ifdef _WIN32
            WSACleanup();
            #endif
            
            Error error("Unable to create socket");
            callBack(error);
            return;
        }
        struct sockaddr_in serverSocketInformation;
        serverSocketInformation.sin_family = AF_INET;
        serverSocketInformation.sin_port = htons(portNumber);
        serverSocketInformation.sin_addr.s_addr = htonl(INADDR_ANY);
        int successCode = bind(serverSocketDescriptor, (struct sockaddr *)&serverSocketInformation, sizeof(serverSocketInformation));
        if(successCode < 0)
        {
            close(serverSocketDescriptor);

            #ifdef _WIN32
            WSACleanup();
            #endif

            char a[101];
            sprintf(a, "Unable to bind socket to port : %d", portNumber);
            Error error(a);
            callBack(error);
            return;
        }
        successCode = ::listen(serverSocketDescriptor, 10); // ::listen (By specifying this i am talking about the global listen function)
        if(successCode < 0)
        {
            close(serverSocketDescriptor);

            #ifdef _WIN32
            WSACleanup();
            #endif
            
            Error error("Unable to accept client connections");
            callBack(error);
            return;
        }
        Error error("");
        callBack(error);
        struct sockaddr_in clientSocketInformation;
        
        #ifdef linux
        socklen_t len = sizeof(clientSocketInformation);
        #endif

        #ifdef _WIN32
        int len = sizeof(clientSocketInformation);
        #endif

        int clientSocketDescriptor;
        while(true)
        {
            clientSocketDescriptor = accept(serverSocketDescriptor, (struct sockaddr *)&clientSocketInformation, &len);
            requestLength = recv(clientSocketDescriptor,requestBuffer,sizeof(requestBuffer)-sizeof(char),0);
            if(requestLength==0 || requestLength==-1)
            {
                close(clientSocketDescriptor);
                continue;
            }
            int e;
            char *method, *requestURI, *httpVersion, *dataInRequest;
            requestBuffer[requestLength] = '\0';
            // code to parse the first line of the http request starts here
            // first line should be REQUEST_METHOD space URI space HTTPVersionCFRLF
            method = requestBuffer;
            e = 0;
            while(requestBuffer[e] && requestBuffer[e]!=' ') e++;
            if(requestBuffer[e]=='\0')
            {
                HttpErrorStatusUtility::sendBadRequestError(clientSocketDescriptor);
                close(clientSocketDescriptor);
                continue;
            }
            requestBuffer[e] = '\0';
            e++;
            if(requestBuffer[e]==' ' || requestBuffer[e]=='\0')
            {
                HttpErrorStatusUtility::sendBadRequestError(clientSocketDescriptor);
                close(clientSocketDescriptor);
                continue;
            }
            StringUtility::toLowerCase(method);

            if(!(strcmp(method,"get")==0 || strcmp(method,"post")==0 ||strcmp(method,"put")==0 ||strcmp(method,"delete")==0 ||strcmp(method,"head")==0 ||strcmp(method,"options")==0 ||strcmp(method,"trace")==0 ||strcmp(method,"connect")==0))
            {
                HttpErrorStatusUtility::sendBadRequestError(clientSocketDescriptor);
                close(clientSocketDescriptor);
                continue;
            }
            requestURI=requestBuffer + e;
            while(requestBuffer[e] && requestBuffer[e]!=' ') e++;
            if(requestBuffer[e]=='\0')
            {
                HttpErrorStatusUtility::sendBadRequestError(clientSocketDescriptor);
                close(clientSocketDescriptor);
                continue;
            }
            requestBuffer[e]='\0';
            e++;
            if(requestBuffer[e]==' ' || requestBuffer[e]=='\0')
            {
                HttpErrorStatusUtility::sendBadRequestError(clientSocketDescriptor);
                close(clientSocketDescriptor);
                continue;
            }
            httpVersion=requestBuffer + e;
            while(requestBuffer[e] && requestBuffer[e] != '\r' && requestBuffer[e]!='\n') e++;
            if(requestBuffer[e]=='\0')
            {
                HttpErrorStatusUtility::sendBadRequestError(clientSocketDescriptor);
                close(clientSocketDescriptor);
                continue;
            }
            if(requestBuffer[e]=='\r' && requestBuffer[e + 1]!='\n')
            {
                HttpErrorStatusUtility::sendBadRequestError(clientSocketDescriptor);
                close(clientSocketDescriptor);
                continue;
            }
            if(requestBuffer[e]=='\r')
            {
                requestBuffer[e]='\0';
                e = e + 2;
            }
            else
            {
                requestBuffer[e]='\0';
                e = e + 1;
            }
            StringUtility::toLowerCase(httpVersion);
            if(strcmp(httpVersion,"http/1.1")!=0)
            {
                HttpErrorStatusUtility::sendHttpVersionNotSupportedError(clientSocketDescriptor,httpVersion);
                close(clientSocketDescriptor);
                continue;
            }
            dataInRequest = NULL;
            e = 0;
            while(requestURI[e] != '\0' && requestURI[e] != '?') e++;
            if(requestURI[e] == '?')
            {
                requestURI[e] = '\0';
                dataInRequest = requestURI + e + 1;
            }

            cout<<"Request arrived, URI is : "<<requestURI<<endl;
            auto urlMappingsIterator=urlMappings.find(requestURI);
            if(urlMappingsIterator==urlMappings.end())
            {
                if(!serveStaticResource(clientSocketDescriptor, requestURI))
                {
                    HttpErrorStatusUtility::sendNotFoundError(clientSocketDescriptor, requestURI);
                }
                close(clientSocketDescriptor);
                continue;
            }
            URLMapping urlMapping = urlMappingsIterator->second;
            if(urlMapping.requestMethod==__GET__ && strcmp(method,"get")!=0)
            {
                HttpErrorStatusUtility::sendMethodNotAllowedError(clientSocketDescriptor,method,requestURI);
                close(clientSocketDescriptor);
                continue;
            }else if(urlMapping.requestMethod==__POST__ && strcmp(method,"post")!=0)
            {
                HttpErrorStatusUtility::sendMethodNotAllowedError(clientSocketDescriptor,method,requestURI);
                close(clientSocketDescriptor);
                continue;
            }
            // code to parse the header and then the payload if exists starts here
            // code to parse the header and then the payload if exists ends here
            Request request(method, requestURI, httpVersion, dataInRequest);
            Response response;
            urlMapping.mappedFunction(request,response);
            HttpResponseUtility::sendResponse(clientSocketDescriptor,response);
            close(clientSocketDescriptor);
            // lot of code will be written here later on
        } // Infinite loop ends here

        #ifdef _WIN32
        WSACleanup();
        #endif
    }
};




// Sam (The User of HTTP Web Server)

int main()
{
    try
    {
        Bro bro;
        bro.setStaticResourcesFolder("static_resources");
        bro.get("/", [](Request &request, Response &response) -> void {
            const char *htmlPage = R""""(
            <!DOCTYPE html>
            <html lang = 'en'>
            <head>
            <meta charset = 'utf-8'>
            <title>whatever</title>
            </head>
            <body>
            <h1>Hey, Welcome</h1>
            <a href = 'getCustomers'>Customers</a>
            </body>
            </html>
            )"""";
            response.setContentType("text/html"); // Setting MIME Type
            response<<htmlPage;
        });

        bro.get("/getCustomers", [](Request &request, Response &response) -> void {
            const char *htmlPage = R""""(
            <!DOCTYPE html>
        <html lang="en">
        <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Dynamic Mesh Configuration</title>
        <style>
            * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
            font-family: Arial, sans-serif;
            }

            html, body {
            height: 100%;
            min-height: 100vh;
            background-color: #f9f9f9;
            display: flex;
            flex-direction: column;
            }

            header {
            background-color: #d21134;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 60px;
            padding: 10px 20px;
            }

            header img {
            max-height: 40px;
            }

            .container {
            flex: 1;
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: flex-start;
            padding: 20px;
            max-width: 900px;
            margin: 0 auto;
            width: 100%;
            }

            h1 {
            color: #d21134;
            margin-bottom: 20px;
            font-size: 22px;
            text-align: center;
            }

            .buttons {
            display: flex;
            flex-wrap: wrap;
            justify-content: center;
            gap: 16px;
            width: 100%;
            }

            .card {
            background-color: white;
            color: #d21134;
            padding: 20px;
            border-radius: 10px;
            border: 2px solid #d21134;
            box-shadow: 0 4px 8px rgba(0,0,0,0.1);
            font-size: 16px;
            font-weight: bold;
            cursor: pointer;
            transition: all 0.3s ease;
            flex: 1 1 280px;
            max-width: 300px;
            text-align: center;
            }

            .card:hover {
            background-color: #d21134;
            color: white;
            transform: translateY(-3px);
            box-shadow: 0 8px 16px rgba(0,0,0,0.15);
            }

            footer {
            background-color: #f1f1f1;
            padding: 10px 0;
            text-align: center;
            color: #333;
            font-size: 14px;
            position: fixed;
            left: 0;
            bottom: 0;
            width: 100%;
            z-index: 1000;
            }

            @media (max-width: 480px) {
            .card {
                font-size: 14px;
                padding: 16px;
            }
            }
        </style>
        </head>
        <body>

        <div class=container>
        <h1>Welcome</h1>
        <div class=buttons>
        <div class=card onclick=location.href='/getParentChildList'>Get Parent Child List</div>
        <div class=card onclick=location.href='/massConfiguration'>Mass Configuration</div>
        <div class=card onclick=location.href='/downLinkConfiguration'>Down Link Configuration</div>
        <div class=card onclick=location.href='/assignManualUplink'>Assign Manual Uplink(s)</div>
        <div class=card onclick=location.href='/spiralMeshConfiguration'>Spiral Mesh Configuration</div>
        <div class=card onclick=location.href='/setFESensorCredentials'>Set FE Sensor Credentials</div>
        </div></div>
        </body>
        </html>
            )"""";
            response.setContentType("text/html"); // Setting MIME Type
            response<<htmlPage;
        });

        bro.get("/save_test1_data", [](Request &request, Response &response) -> void {
            string nnn = request["nm"];
            string ccc = request["ct"];
            cout<<"Data that arrived in request"<<endl;
            cout<<nnn<<endl;
            cout<<ccc<<endl;
            cout<<"-----------------------------------------------------"<<endl;
            const char *htmlPage = R""""(
            <!DOCTYPE html>
            <html lang = 'en'>
            <head>
            <meta charset = 'utf-8'>
            <title>Server Testcases</title>
            </head>
            <body>
            <h1>Testcase 1 - GET with query string</h1>
            <h3>Response from server side</h3>
            <b>Data Saved</b>
            <br/><br/>
            <a href = '/server_testing.html'>Home</a>
            </body>
            </html>
            )"""";
            response.setContentType("text/html"); // Setting MIME Type
            response<<htmlPage;
        });

        bro.post("/save_test2_data", [](Request &request, Response &response) -> void {
            const char *htmlPage = R""""(
            <!DOCTYPE html>
            <html lang = 'en'>
            <head>
            <meta charset = 'utf-8'>
            <title>Server Testcases</title>
            </head>
            <body>
            <h1>Testcase 2 - POST with form data</h1>
            <h3>Response from server side</h3>
            <b>Data Saved</b>
            <br/><br/>
            <a href = '/server_testing.html'>Home</a>
            </body>
            </html>
            )"""";
            response.setContentType("text/html"); // Setting MIME Type
            response<<htmlPage;
        });

        bro.listen(6060, [](Error &error) -> void {
            if(error.hasError())
            {
                cout<<error.getError()<<endl;
                return;
            }
            cout<<"Bro HTTP Server is successfully listening on port number 6060"<<endl;
        });
    }catch(string exception)
    {
        cout<<exception<<endl;
    }
    return 0;
}