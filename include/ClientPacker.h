template <typename T>
char* packDataWithHeader(T& data, char* description)
{
    // Calculate the size of the data
    size_t dataSize = sizeof(T);

    // Calculate the packet size including the header
    uint32_t packetSize = 4 + 12 + dataSize;

    if (packetSize > 4096)
        return nullptr;

    // Allocate memory for the buffer
    char* buffer = new char[packetSize];

    // Copy the size into the buffer
    memcpy(buffer, (char*)&packetSize, 4);

    // Copy the discrption into the buffer after the size field
    memcpy(buffer + 4, description, 12);

    // Copy te data into the buffer after the complete header
    memcpy(buffer + 4 + 12, reinterpret_cast<const char*>(&data), dataSize);

    return buffer;
}

int unpackSize (char* buffer)
{
    // Extract the packet size from the buffer
    int packetSize;
    memcpy((char*)&packetSize, buffer, 4);
    
    return packetSize;
}

char* unpackDescription (char* buffer)
{
    // Buffer for description
    char* description = new char [12 + 1];

    // Extract the description from the buffer
    memcpy(description, buffer + 4, 12);
    description[12] = '\0';

    return description;
}

template <typename T>
bool unpackData(char* buffer, T* data, int size)
{
    // Size of data
    int dataSize = size - (4 + 12);

    if (dataSize != sizeof(T))
        return false;

    // Extract the data from the buffer
    memcpy((char*)data, buffer + 4 + 12, dataSize);

    return true;
}
