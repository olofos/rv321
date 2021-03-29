import sys
import xml.etree.ElementTree as ET

def load(filename):
    root = ET.parse(filename)

    tests = []
    testroots = root.findall("./visualElements/visualElement[elementName='Testcase']")
    for testroot in testroots:
        labelroot = testroot.find("./elementAttributes/entry[string='Label']")
        if labelroot:
            name = labelroot.findall("./string")[1].text
        else:
            name = 'Unnamed test'
        testcase = testroot.find("./elementAttributes/entry[string='Testdata']/testData/dataString").text
        tests.append({'name': name, 'test': testcase})
    return tests

if __name__ == "__main__":
    print(load(sys.argv[1]))
