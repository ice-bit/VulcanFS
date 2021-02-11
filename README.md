# VulcanFS
VulcanFS is a file system generator for VulcanOS written in C
for the [VulcanOS](https://github.com/ice-bit/vulcanos) kernel.  
This software is a slight different version of [James Molloy's file system generator](http://www.jamesmolloy.co.uk/). You can find a brief explanation of how it is being designed below.  

## Design
VulcanFS does not supports subdirectories nor file permissions(yet) and it only supports
a fixed amount of files(i.e., no more than 128 files). The file system structure 
looks like this:

![scheme](scheme.png)

That is, the _header_ is divided in two slices: the first 8 bytes represents the number
of files while the remaining space is being used by the files headers(the name, size and other metadata of each files). The remaining space contains the actual file data.


## Implementation
To implement this file system we can just follow this algorithm:
1. For each file, write the header on the file system image;  
2. Copy file content into the file system image.

To do this, we also need to define which fields will be inside the file headers structure. At this stage, we can just stick to the following values:
|                Field               |      Size     |
|:----------------------------------:|:-------------:|
|              File name             | `array of char` |
|           Length of file           |    `4 bytes`    |
|         Offset of each file        |    `4 bytes`    |
| Magic number for consistency check |     `2 byte`    |


We can obtain this using the following structure:
```c
struct vulcanfs {
    char name[64]; // Maximum name size can be adjusted
    unsigned int length;
    unsigned int offset;
    unsigned short magic_number;
}
````

