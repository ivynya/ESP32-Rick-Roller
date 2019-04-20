class MinifyHTML(object):
    def __init__(self, htmlFilename, escapeCharacters):
        htmlLines = open(htmlFilename, 'r').readlines()
        minifiedHTML = self.minify(htmlLines)
        if (escapeCharacters):
            print("")
            print(self.escape(minifiedHTML))
        else:
            print("")
            print(minifiedHTML)

    def minify(self, htmlLines):
        result = ""
        for line in htmlLines:
            result += line.strip()
        return result

    def escape(self, html):
        html = html.replace("'", "\\'")
        html = html.replace('"', '\\"')
        return html

while(True):
    MinifyHTML(input("Filename > "), bool(input("Escape Characters? [True/False] > ")))
    print("")
