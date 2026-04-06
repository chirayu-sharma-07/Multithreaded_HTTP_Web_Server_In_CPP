#include<iostream>
#include<map>
#include<forward_list>
#include<string.h>
#include<unistd.h>

#ifdef _WIN32
#include<windows.h>
#endif

#ifdef linux
#include<arpa/inet.h>
#include<sys/socket.h>
#endif

using namespace std;

// Chirayu (The Creator of HTTP Web Server)

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
        return true;
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

    public:

    Bro()
    {

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

        }
    }

    void get(string url, void (*callBack)(Request &, Response &))
    {
        if(Validator::isValidURLFormat(url))
        {
            urlMappings.insert(pair<string, URLMapping>(url, {__GET__, callBack}));
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
            if(clientSocketDescriptor < 0)
            {
                // Not yet decided, will write this code later on
            }
            forward_list<string> requestBufferDS;
            forward_list<string>::iterator requestBufferDSIterator;
            requestBufferDSIterator = requestBufferDS.before_begin();
            int requestBufferDSSize = 0;
            int requestDataCount = 0;
            while(true)
            {
                requestLength = recv(clientSocketDescriptor, requestBuffer, sizeof(requestBuffer) - sizeof(char), 0);
                if(requestLength == 0) break;
                requestBuffer[requestLength] = '\0';
                requestBufferDSIterator = requestBufferDS.insert_after(requestBufferDSIterator, string(requestBuffer));
                requestBufferDSSize++;
                requestDataCount += requestLength;
            }
            if(requestBufferDSSize > 0)
            {
                char *requestData = new char[requestDataCount + 1];
                char *p;
                p = requestData;
                const char *q;
                requestBufferDSIterator = requestBufferDS.begin();
                while(requestBufferDSIterator != requestBufferDS.end())
                {
                    q = (*requestBufferDSIterator).c_str();
                    while(*q)
                    {
                        *p = *q;
                        p++;
                        q++;
                    }
                    ++requestBufferDSIterator;
                }
                *p = '\0';
                requestBufferDS.clear();
                printf("----------------- request data begin ---------------------\n");
                printf("%s\n", requestData);
                printf("----------------- request data end ---------------------\n");
                // The code to parse the request goes here

                // code to extract the URL
                // code to look for the URL in urlMappings map
                // if not found prepare header with 404 and send back the header
                // if found :-
                Request request;
                Response response;
                // place a call to the method whose address is the value part in the map
                // the value part against the key is the address of the function that was mapped with the url by sam
                // when the call ends create header with 200 ok and content length as per the value of contentLength property in the response object
                // write header
                // iterate the forward_list named as content
                // on every cycle write bytes

                delete [] requestData;
            }
            else
            {
                // something if no data was received
            }
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
    Bro bro;
    bro.setStaticResourcesFolder("whatever the folder name is");
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
        <html lang = 'en'>
        <head>
        <meta charset = 'utf-8'>
        <title>Customers</title>
        </head>
        <body>
        <h1>List of customers</h1>
        <ul>
        <li>Gautam</li>
        <li>Utsav</li>
        <li>Shivansh</li>
        <li>Madhur</li>
        </ul>
        <a href = '/'>Home</a>
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
        cout<<"Bro Server is successfully listening on port number 6060"<<endl;
    });
    return 0;
}