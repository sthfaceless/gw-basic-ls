import path from "path";
const fs = require('fs');

let configPath = path.resolve(__dirname, '..', 'data', 'config.json');
let config = JSON.parse(fs.readFileSync(configPath));

function add_keyword(name, type, purpose, syntax, kind){
    let exist = false;
    for (let keyword of config.keywords)
        if(keyword.name === name)
            exist = true;
    if(!exist){
        config.keywords.push({
            name: name,
            type: type,
            args: 0,
            purpose: purpose,
            syntax: syntax,
            kind: kind
        });
    }
}

add_keyword("mod", "arithmetic operator", "Get the remainder of the division", "a mod b", 21);
add_keyword("not", "logical operator", "Logical negation", "NOT a", 21);
add_keyword("and", "logical operator", "Logical and", "a AND b", 21);
add_keyword("or", "logical operator", "Logical or", "a OR b", 21);
add_keyword("xor", "logical operator", "Logical xor", "a XOR b", 21);
add_keyword("eqv", "logical operator", "Logical equivalence", "a EQV b", 21);
add_keyword("imp", "logical operator", "Logical implication", "a IMP b", 21);
add_keyword("rem", "comment delimiter", "To make comments", "REM comment", 15);

fs.writeFileSync(configPath, JSON.stringify(config, null, 4));

