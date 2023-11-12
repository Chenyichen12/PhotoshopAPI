# PhotoshopAPI

## Project

- [X] Generate CMake project for PhotoshopAPI
- [ ] Include blosc2 compression library
- [ ] Add README

## Testing

- [X] Add wide-range of test cases for testing based on photoshop files (both psd and psb)
- [ ] Add unit testing for low-level components such as reading from a binary file
 
## PhotoshopFile

- [X] Read File Header
- [X] Read ColorModeData
- [X] Read Image Resources
- [ ] Read Layer and Mask information
    - [X] Read Global Layer Mask Info
    - [X] Read Layer Records
        - [X] Add parsing of tagged blocks 
    - [ ] Read Channel Image Data
        - [ ] Compress using blosc2
    - [ ] Read Additional Layer Information
- [ ] Read Merged Image Data Section
    - [ ] Compress using blosc2

## LayeredFile