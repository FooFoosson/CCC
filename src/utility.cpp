#include "utility.h"

ccc::SharedBuffer::SharedBuffer()
    : count { 0 }
{
}

ccc::SharedBuffer::~SharedBuffer()
{
    while (!buffer.empty()) {
        delete buffer.front();
        buffer.pop();
    }
}

void ccc::SharedBuffer::produce(Token* token)
{
    // if lock guard used consume could resume before the lock is released 
    // i.e. method returns which would cause wait to sleep again 
    // since it first tries to lock the mutex and awaken once the lock is released
    // needlessly wasting cycles
    m.lock();
    buffer.push(token);
    ++count;
    m.unlock();
    condition.notify_one();
}

ccc::Token* ccc::SharedBuffer::consume()
{
    std::unique_lock<std::mutex> lock { m };
    condition.wait(lock, [this]() { return count > 0; });
    Token* res = buffer.front();
    return res;
}

void ccc::SharedBuffer::pop()
{
    std::lock_guard<std::mutex> lock { m };
    if (buffer.empty())
        return;
    delete buffer.front();
    --count;
    buffer.pop();
}
