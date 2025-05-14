#ifndef CONNECTIONS_STRUCTURE_H
#define CONNECTIONS_STRUCTURE_H

#include <cmath>
#include <cstdint>
#include <iostream>
#include <vector>

struct Connection
{
  int fd = -1;
  bool want_read = false;
  bool want_write = false;
  bool want_close = false;

  std::vector<uint8_t> incoming;
  std::vector<uint8_t> outgoing;
};

class Buffer
{
public:
  uint8_t *begin; // This pointer will be assigned to an array
  uint8_t *end;
  uint8_t *data_begin;
  uint8_t *data_end;

  void consume(size_t len)
  {
    size_t available_data = this->size();

    if (len > available_data)
    {
      this->data_begin = this->data_end;
      if (len > 0)
      {
        std::cerr << "Warning: Buffer::consume() - Attempt to consume more data than available [attempted: " << len
                  << "bytes, available: " << available_data << "bytes]" << std::endl;
      }
    }
    else
    {
      this->data_begin += len;
    }

    // Reset the data pointers to begin buffer if all data has been consumed.
    // With this, we can reuse the buffer without requiring a full compaction cycle immediately.
    if (this->data_begin == this->data_end)
    {
      this->data_begin = this->begin;
      this->data_end = this->begin;
    }
  }

  void append(const uint8_t *data, size_t len)
  {
    if (len == 0)
    {
      return;
    }

    if (data == nullptr)
    {
      std::cerr << "Error: Buffer::append() - Cannot append from a null data pointer." << std::endl;
      return;
    }

    if (free_space_at_end() >= len)
    {
      this->push_raw(data, len);
    }
    else
    {
      if (this->free_space() >= len)
      {
        this->compact();
        this->push_raw(data, len);
      }
      else
      {
        size_t current_size = this->size();
        size_t new_capacity = this->get_new_capacity(len);

        uint8_t *new_begin = new (std::nothrow) uint8_t[new_capacity];
        if (!new_begin)
        {
          std::cerr << "Error: Buffer::append() - Failed to allocate new buffer." << std::endl;
          return;
        }

        if (current_size > 0)
        {
          memcpy(new_begin, this->data_begin, current_size);
        }

        delete[] this->begin;

        this->begin = new_begin;
        this->end = new_begin + new_capacity;
        this->data_begin = new_begin;
        this->data_end = new_begin + current_size;

        this->push_raw(data, len);
      }
    }
  }

  void compact()
  {
    size_t current_size = this->size();
    if (current_size > 0)
    {
      memmove(this->begin, this->data_begin, current_size);
    }

    this->data_begin = this->begin;
    this->data_end = this->begin + current_size;
  }

  size_t size() const
  {
    return this->data_end - this->data_begin;
  }

  size_t capacity() const
  {
    return this->end - this->begin;
  }

  ~Buffer()
  {
    delete[] begin;
    delete end;
    delete data_begin;
    delete data_end;
  }

private:
  size_t free_space_at_begin() const
  {
    return this->data_begin - this->begin;
  }

  size_t free_space_at_end() const
  {
    return this->end - this->data_end;
  }

  size_t free_space() const
  {
    return this->free_space_at_begin() + this->free_space_at_end();
  }

  size_t get_new_capacity(size_t len) const
  {
    size_t required_capacity = this->size() + len;
    size_t current_capacity = this->capacity();
    return current_capacity == 0
               ? required_capacity
               : std::max(current_capacity * 2, required_capacity);
  }

  void push_raw(const uint8_t *data, size_t len)
  {
    memcpy(this->data_end, data, len);
    this->data_end += len;
  }
};

#endif
