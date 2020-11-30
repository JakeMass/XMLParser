#include <bits/c++config.h>
#include <iostream>
#include <fstream>
#include <ostream>
#include <sched.h>
#include <string>
#include <vector>

struct Attrib {
  std::string key, value;
};

class Tag {
private:
  std::string name;
  std::string start_tag;
  std::string end_tag;
  std::string content;
  std::string text;
  Tag* parent_tag;
  std::vector<Tag> children;
  std::vector<Attrib> attribs;

public:
  Tag()
  {
    name = "";
    start_tag = "";
    end_tag = "</" + name + ">";
    text = "";
    parent_tag = nullptr;
  }

  Tag(std::string tag_string, Tag* parent_tag)
  {
    start_tag = tag_string;
    std::size_t close_tag_pos = tag_string.find('>');

    for(int i = 1; i < close_tag_pos; i++){
      if(tag_string[i] == ' ' || tag_string[i] == '/'){
        break;
      }
      else {
        name += tag_string[i];
      }
    }

    end_tag = "</" + name + ">";
    text = "";
    this->parent_tag = parent_tag;
  }

  std::string get_start_tag()
  {
    return start_tag;
  }

  std::string get_name()
  {
    return name;
  }

  std::string get_end_tag()
  {
    return end_tag;
  }

  std::string get_text()
  {
    return text;
  }

  std::string get_content()
  {
    return content;
  }

  Tag* get_parent()
  {
    return parent_tag;
  }

  std::string* get_attrib_keys()
  {
    int size = attribs.size();
    std::string keys[size];

    for(int i = 0; i < size; i++)
    {
      keys[i] = attribs[i].key;
    }
    return keys;
  }

  void set_start_tag(std::string start_tag)
  {
    this->start_tag = start_tag;
  }

  void set_content(std::string content)
  {
    this->content = content;
  }

  void set_name(std::string name)
  {
    this->name = name;
  }

  void make_children()
  {
    for(int i = 0; i < content.length(); i++)
    {
      if(content[i] == '<')
      {
        std::size_t close_tag_pos = content.find('>', i);
        std::string tag_string = "";

        for(; i <= close_tag_pos; i++){
          tag_string += content[i];
        }
        //std::cout << tag_string << "\n";
        std::size_t empty_tag_pos = tag_string.find("/>");
        Tag new_child (tag_string, this);
        std::string new_child_content = "";

        if(empty_tag_pos == std::string::npos){
          std::size_t child_close_tag_pos = content.find(new_child.get_end_tag(), i);

          if(child_close_tag_pos != std::string::npos){
            for(; i < child_close_tag_pos; i++){
              new_child_content += content[i];
            }
          }
          //std::cout << new_child.get_name() << "<" << new_child.get_parent()->get_name() << ">" << " content: "<< new_child_content << "\n";   
        }
        new_child.init(new_child_content);
        children.push_back(new_child);
      }
    }
    //std::cout << name << " text: " << text << "\n";
  }

  void make_text()
  {
    if(content[0] == '<'){
      std::size_t last_closing_tag_pos = content.find_last_of('>');
      int start;

      if(last_closing_tag_pos == std::string::npos){
        start = 1;
      }
      else {
        start = last_closing_tag_pos + 1;
      }

      for(int i = start; i < content.length(); i++)
      {
        text += content[i];
      }
    }
    else {
      std::size_t first_opening_tag_pos = content.find('<');
      int end;

      if(first_opening_tag_pos == std::string::npos)
      {
        end = content.length();
      }
      else {
        end = first_opening_tag_pos;
      }

      for(int i = 0; i < end; i++)
      {
        text += content[i];
      }
    }
    //std::cout << name << " text: " << text << "\n";
  }

  void make_attributes()
  {
    std::size_t attrib_begin = start_tag.find(' ');

    if(attrib_begin != std::string::npos){
      for(int i = attrib_begin + 1; i < start_tag.length(); i++){
        std::size_t equal_pos = start_tag.find('=', i);
        if(equal_pos != std::string::npos){
          Attrib attrib;

          for(; i < equal_pos; i++)
          {
            attrib.key += start_tag[i];
          }

          for(i += 2; start_tag[i] != '"'; i++){
            attrib.value += start_tag[i];
          }

          attribs.push_back(attrib);
        }
      }
    }
  }

  void init(std::string content)
  {
    set_content(content);
    make_children();
    make_text();
    make_attributes();
  }

  void print_child_names()
  {
    std::cout << name << " children names: ";

    for(int i = 0; i < children.size(); i++)
    {
      std::cout << children[i].get_name() << ", ";
    }
    std::cout << "\n";
  }

  void print_attribs()
  {
    std::cout << name << " attributes: ";

    for(int i = 0; i < attribs.size(); i++){
      std::cout << attribs[i].key << ": " << attribs[i].value << "\n";
    }
    std::cout << "\n";
  }

  Tag* find_tag(std::string search_value)
  {
    Tag* child;

    for(int i = 0; i < children.size(); i++)
    {
      if(children[i].get_name() == search_value)
      {
        child = &children[i];
        break;
      }
    }

    return child;
  }

  Tag* find_attrib(std::string search_value)
  {
    Tag* child;

    for(int i = 0; i < children.size(); i++)
    {
      
    }
  }
};

std::string make_tag_content(Tag* tag_ptr, std::ifstream* file_ptr, std::string* file_string_ptr)
{
  std::string tag_content = "";
  if(file_ptr->is_open())
  {
    while(getline(*file_ptr, *file_string_ptr))
    {
        tag_content += *file_string_ptr;
    }
    tag_ptr->set_content(tag_content);
  }
  return tag_content;
}

class FileTree {
private:
  Tag root_tag;
  std::string root_end_tag;
  std::string file_name;

public:
  Tag make_root()
  {
    std::ifstream file (file_name);
    std::string root_tag_string;
    std::string file_string;

    if(file.is_open())
    {
      getline(file, file_string, '>');
      root_tag_string = file_string + '>';
    }

    Tag output (root_tag_string, nullptr);

    std::string root_content = "";
    std::string tmp_content = make_tag_content(&output, &file, &file_string);

    for(int i = 0; i < tmp_content.length() - output.get_end_tag().length(); i++)
    {
      root_content += tmp_content[i];
    }

    output.init(root_content);
    file.close();
    //std::cout << output.get_content() << "\n";
    return output;
  }

  FileTree(std::string file_name)
  {
    this->file_name = file_name;
    root_tag = make_root();
    root_end_tag = root_tag.get_end_tag();
  }

  Tag* get_root()
  {
    return &root_tag;
  }

};

int main(){
  FileTree tree ("test.xml");
  Tag* root = tree.get_root();

  std::cout << root->find_tag("test3")->get_name() << "\n";

  return 0;
}
