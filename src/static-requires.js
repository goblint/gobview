var prismJs = require("./static/prism.js");
var prismCss = require("./static/prism.css");

require('./stylesheet.scss');

joo_global_object.monaco = require('monaco-editor/esm/vs/editor/editor.api');
require('monaco-editor/esm/vs/basic-languages/cpp/cpp.contribution');
require('monaco-editor/esm/vs/editor/contrib/hover/hover');

var autocompleteBS = require("bootstrap5-autocomplete");
joo_global_object.autocompleteBS = autocompleteBS;
