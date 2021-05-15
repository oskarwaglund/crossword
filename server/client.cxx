#include "client.h"

Client::Client(int socket, string name) : socket{socket}, name{name}{}

Client::~Client(){}