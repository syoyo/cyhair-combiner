# Combine multiple cyhair files

## Requirements

* C++11 compiler

## Input JSON file format

Describes array of filename and hair parameters.

Here is an example:

```
[ { "filename" : "a0.hair", "thickness" : 0.1 },
  { "filename" : "a1.hair", "thickness" : 0.2 }
]
```

## Usage

Edit 


```
$ ./cyhair-combine input.json
```

## License

MIT license.

### Third party licenses

* json.hpp JSON for Modern C++. MIT license. https://github.com/nlohmann/json

