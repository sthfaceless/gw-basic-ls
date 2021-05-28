const request = require("request"),
    fs = require('fs');
import * as path from 'path';
import {parse} from "node-html-parser";

const URI = "http://www.antonis.de/qbebooks/gwbasman/";
let configPath = path.resolve(__dirname, '..', 'data', 'config.json');
let config = JSON.parse(fs.readFileSync(configPath));

const types = new Set(['statement', 'function', 'variable', 'command']);

function getNameType(tokens){
    let name = "", type = "";
    for (const token of tokens) {
        if (types.has(token)) {
            type = token;
        } else if (!type) {
            name += token + " ";
        }
    }
    name = name.trim();
    return {name, type};
}

function addMoreInfo(href, info) {
    request(URI + href, (error, response, body) => {
        if (!error) {

            console.log(href);

            const description = parse(body);
            let purpose = description.querySelectorAll('p')[0].innerText.replace(/<[^>]*>/g, "");

            let syntaxes = description.querySelectorAll('pre')[0].innerText.replace(/<[^>]*>/g, "").toLowerCase().split('\n');
            let syntax = syntaxes[0].replace(/^v=/g, "");
            for(let _syntax of syntaxes) {
                if (_syntax.includes(info.name))
                    syntax = _syntax.replace(/^v=/g, "");
            }

            if(info.type === 'function' && new RegExp("^"+info.name+"(\\((.+?,)*?.+?\\))?$", "g").test(syntax)){
                let pattern = '^'+info.name+'(\\((.+?,)*?.+?\\))?$';
                let args = Number(syntax.includes('(')) + syntax.split(',').length-1;
                info = {...info, args, pattern};
            }

            info = {...info, purpose, syntax, kind: info.type === 'variable' ? 13 : 12};
            config.keywords.push(info);
        } else {
            console.log("Произошла ошибка: " + error);
        }
    });
}

function process_keywords(list, curr){
    if(curr < list.length){
        const element = list[curr];
        const tokens = element.innerText.toLowerCase().split(' '),
            href = element.attrs['href'];

        let info = getNameType(tokens);
        addMoreInfo(href, info);

        setTimeout(() => process_keywords(list, curr+1), 1000);
    }
}

request(URI + "index2.html", (error, response, body) => {
    if (!error) {
        const root = parse(body);
        const list = root.querySelectorAll('font[size="-1"]')[1].querySelectorAll('a');

        config.keywords = [];
        process_keywords(list, 0);

    } else {
        console.log("Произошла ошибка: " + error);
    }
});

setTimeout(()=> fs.writeFileSync(configPath, JSON.stringify(config, null, 4)),
    5 * 60 * 1000);
