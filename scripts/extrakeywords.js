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

/*
* String operators
* */

add_keyword("mod", "arithmetic operator", "Get the remainder of the division", "a mod b", 23);
add_keyword("not", "logical operator", "Logical negation", "NOT a", 24);
add_keyword("and", "logical operator", "Logical and", "a AND b", 24);
add_keyword("or", "logical operator", "Logical or", "a OR b", 24);
add_keyword("xor", "logical operator", "Logical xor", "a XOR b", 24);
add_keyword("eqv", "logical operator", "Logical equivalence", "a EQV b", 24);
add_keyword("imp", "logical operator", "Logical implication", "a IMP b", 24);
add_keyword("rem", "comment delimiter", "To make comments", "REM comment", 15);

/*
* Math function
* */
add_keyword("sec", "Secant", "Compute secant", "SEC(X)", 12);
add_keyword("csc", "Cosecant", "Compute Cosecant", "CSC(X)", 12);
add_keyword("cot", "Cotangent", "Compute Cotangent", "COT(X)", 12);
add_keyword("arcsin", "Inverse sine", "Compute Inverse sine", "ARCSIN(X)", 12);
add_keyword("arccos", "Inverse cosine", "Compute Inverse cosine", "ARCCOS(X)", 12);
add_keyword("arcsec", "Inverse secant", "Compute Inverse secant", "ARCSEC(X)", 12);
add_keyword("arccsc", "Inverse cosecant", "Compute Inverse cosecant", "ARCCSC(X)", 12);
add_keyword("arccot", "Inverse cotangent", "Compute Inverse cotangent", "ARCCOT(X)", 12);
add_keyword("sinh", "Hyperbolic sine", "Compute Hyperbolic sine", "SINH(X)", 12);
add_keyword("cosh", "Hyperbolic cosine", "Compute Hyperbolic cosine", "COSH(X)", 12);
add_keyword("tanh", "Hyperbolic tangent", "Compute Hyperbolic tangent", "TANH(X)", 12);
add_keyword("sech", "Hyperbolic secant", "Compute Hyperbolic secant", "SECH(X)", 12);
add_keyword("csch", "Hyperbolic cosecant", "Compute Hyperbolic cosecant", "CSCH(X)", 12);
add_keyword("coth", "Hyperbolic cotangent", "Compute Hyperbolic cotangent", "COTH(X)", 12);
add_keyword("arcsinh", "Inverse Hyperbolic sine", "Compute Hyperbolic sine", "ARCSINH(X)", 12);
add_keyword("arccosh", "Inverse Hyperbolic cosine", "Compute Hyperbolic cosine", "ARCCOSH(X)", 12);
add_keyword("arctanh", "Inverse Hyperbolic tangent", "Compute Hyperbolic tangent", "ARCTANH(X)", 12);
add_keyword("arccsch", "Inverse Hyperbolic secant", "Compute Hyperbolic secant", "ARCCSCH(X)", 12);
add_keyword("arcsech", "Inverse Hyperbolic cosecant", "Compute Hyperbolic cosecant", "ARCSECH(X)", 12);
add_keyword("arccoth", "Inverse Hyperbolic cotangent", "Compute Hyperbolic cotangent", "ARCCOTH(X)", 12);


function generate_extra_keywords(){
    for(const keyword of config.keywords){
        const name = keyword.name;
        const keywords = name.split(/(\s|-)/g);
        if(keywords.length > 1)
            for(const word of keywords)
                if(!/(\s|-)/g.test(word)){
                    add_keyword(word, "keyword", "gwbasic keyword", word, 15);
                }
    }
}

generate_extra_keywords();
config.keywords = config.keywords.filter((keyword) => !/(-)/g.test(keyword.name));


fs.writeFileSync(configPath, JSON.stringify(config, null, 4));

