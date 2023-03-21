#include <iostream>
#include <unordered_map>
#include <fstream>
#include <queue>
#include <algorithm>
#include <math.h>
using namespace std;
#define op cout

class binary_tree //  Huffman Tree
{
public:
    char value;    // Represents Node of Tree with character present in "file.txt"
    int frequency; // Occurance of each character in "file.txt"
    binary_tree *left;
    binary_tree *right;
    binary_tree(char val, int freq)
    {
        this->value = val;
        this->frequency = freq;
        this->left = nullptr;
        this->right = nullptr;
    }
};

class Compare // Comparator class  to compare the 2 nodes of Binary class according to their frequency.
{
public:
    bool operator()(binary_tree *n1, binary_tree *n2)
    {
        return n1->frequency > n2->frequency;
    }
};

class HuffmanCode  // Huffman Coding Algorithm
{
private:
    priority_queue<binary_tree *, vector<binary_tree *>, Compare> min_heap;
    unordered_map<char, string> code_map;
    unordered_map<string, char> reverse_code_map;
    void frequency_from_text(string text, unordered_map<char, int> &freq_map)
    {
        for (int i = 0; i < text.size(); ++i)
        {
         
            freq_map[text[i]]++;
        }
    }
    void build_heap(unordered_map<char, int> &freq_map)
    {
        int freq;
        for (auto ptr : freq_map)
        {
            freq = freq_map[ptr.first];
            binary_tree *node = new binary_tree(ptr.first, freq);
            min_heap.push(node);
        }
    }
    void build_binary_tree()
    {
        while (min_heap.size() > 1)
        {
            binary_tree *left_node = min_heap.top();
            min_heap.pop();
            binary_tree *right_node = min_heap.top();
            min_heap.pop();
            int total_freq = right_node->frequency + left_node->frequency;
            binary_tree *new_node = new binary_tree('\0', total_freq);
            new_node->left = left_node;
            new_node->right = right_node;
            min_heap.push(new_node);
        }
        return;
    }

    void code_map_from_tree(binary_tree *root, string bit)
    {
        if (root == nullptr)
        {
            return;
        }
        if (root->value != '\0')
        {
            code_map[root->value] = bit;
            reverse_code_map[bit] = root->value;
            return;
        }
        code_map_from_tree(root->right, bit + "1");
        code_map_from_tree(root->left, bit + "0");
    }

    void Build_Tree_Code()
    {
        binary_tree *root = min_heap.top();
        code_map_from_tree(root, "");
    }

    string build_encoded_text(string text)
    {
        string encoded_text = "";
        for (int i = 0; i < text.size(); i++)
        {
            encoded_text.append(code_map[text[i]]);
        }
        return encoded_text;
    }

    void show()
    {
        for (auto p : code_map)
        {
            cout << p.first << " " << p.second << endl;
        }
        for (auto p : reverse_code_map)
        {
            cout << p.first << " " << p.second << endl;
        }
    }
    string build_padded_text(string encoded_text)
    {
        int pad_value = 8 - (encoded_text.size() % 8); // How much bit require to me multiple of 8
        encoded_text.append(pad_value, '0');           // adding require bits to encoded text
        string to_binary;                              // convert the added bit to 8 bit number
        while (pad_value > 0)
        { //  logic to convert decimal to binary
            to_binary += to_string(pad_value % 2);
            pad_value = pad_value / 2;
        }
        int bit_for_pad_info = 8 - to_binary.size(); // binary number should have 8 bit then adding remaining '0's to it
        to_binary.append(bit_for_pad_info, '0');
        reverse(to_binary.begin(), to_binary.end()); // logic has to be reverse in order to get correct binary number
        to_binary.append(encoded_text);              // finally adding padded bits number as binary of 8 bit before encoded bits.
        return to_binary;
    }
    vector<int> build_byte_data(string padded_text)
    {
        vector<int> byte_data;
        vector<string> vec;
        int len = padded_text.size();
        int i{0};
        while (len > 0)
        {
            string sub = padded_text.substr(i, 8); // pos & len
            vec.push_back(sub);
            i = i + 8;
            len = len - 8;
        }

        for (auto p : vec)
        {
            int num{0};
            for (int i = 0; i < p.size(); i++)
            {
                num += (int(p[i]) - 48) * pow(2, p.size() - i - 1);
            }
            byte_data.push_back(num);
        }
        return byte_data;
    }

    string Binary_text(vector<int> &vec)
    {
        string binary_text = "";
        for (auto p : vec)
        {

            string to_binary;
            while (p > 0)
            {
                to_binary += to_string(p % 2);
                p = p / 2;
            }
            if (to_binary.size() != 8)
            {
                int pad_value = 8 - (to_binary.size() % 8);
                to_binary.append(pad_value, '0');
            }
            reverse(to_binary.begin(), to_binary.end());
            binary_text.append(to_binary);
        }
        return binary_text;
    }
    string Remove_Padding(string binary_data)
    {
        string padding_bits = binary_data.substr(0, 8);
        int padding_value{0};
        for (int i = 0; i < padding_bits.size(); i++)
        {
            padding_value += (int(padding_bits[i]) - 48) * pow(2, padding_bits.size() - i - 1);
        }
        return binary_data.substr(8, binary_data.length() - 8 - padding_value);
    }

    string Remove_encoding(string data_after_removing_padding)
    {
        string text = "";
        string current_bit = "";
        for (int i = 0; i < data_after_removing_padding.size(); ++i)
        {
            current_bit += data_after_removing_padding[i];
            if (reverse_code_map.find(current_bit) != reverse_code_map.end())
            {
                text += reverse_code_map[current_bit];
                current_bit = "";
            }
        }

        return text;
    }

public:
    string path;
    string text, encoded_text, padded_text;
    vector<int> byte_data;
    unordered_map<char, int> freq_map{};


    HuffmanCode(string path)
    {
        this->path = path;
    }

    void compress()
    {
        fstream in(path, ios::in);
        while (in.eof() == 0)
        {
            string s;
            getline(in, s);
            text.append(s);
        }
        frequency_from_text(this->text, freq_map);
        build_heap(freq_map);
        build_binary_tree();
        Build_Tree_Code();
        encoded_text = build_encoded_text(text);
        padded_text = build_padded_text(encoded_text);
        byte_data = build_byte_data(padded_text);
        ofstream out("file.bin", ios::binary);
        out.write(reinterpret_cast<const char *>(byte_data.data()), byte_data.size() * sizeof(int));
        out.close();
    }

    void decompress()
    {
        string binary_data, data_after_removing_padding, acutal_text;
        ifstream in("file.bin", ios::binary);
        in.seekg(0, ios::end);
        streampos size = in.tellg();
        in.seekg(0, ios::beg);
        vector<char> buffer(size);
        in.read(buffer.data(), size);
        in.close();

        vector<int> data_from_file(reinterpret_cast<int *>(buffer.data()), reinterpret_cast<int *>(buffer.data() + buffer.size()));
        binary_data = Binary_text(data_from_file);
        data_after_removing_padding = Remove_Padding(binary_data);
        acutal_text = Remove_encoding(data_after_removing_padding);
       

        ofstream fout;
        fout.open("decompressed.txt");
        fout << acutal_text;
        fout.close();
    }
};
int main()
{
    HuffmanCode *ptr = new HuffmanCode("file.txt");
    ptr->compress();
    ptr->decompress();

    return 0;
}

