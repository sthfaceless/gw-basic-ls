let keywords = ["on-sub", "print", "on-gosub"];
keywords = keywords.filter((word) => !/(-)/g.test(word));
console.log(keywords);
