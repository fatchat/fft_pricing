#!/usr/local/bin/python2.6

class Param:
    def __init__(self, line):
        (type, name, start, end, step) = line.split(' ')
        if step == None:
            raise Exception("bad input line: [%s]\n" % line)
        self.name = name.strip()
        if (type == "int"):
            vtype = int
        elif (type == "double"):
            vtype = float
        else:
            raise Exception("unknown type " + type)
        try:
            self.start = vtype(start)
            self.end = vtype(end)
            self.step = vtype(step)
        except ValueError:
            raise Exception("error converting to vtype, type=" + type)

def frange(start, end, step):
    """A float range generator."""
    if step == None:
        raise Exception("array_range requires step (third) parameter")

    if step == 0.0:
        raise ValueError, "step must not be zero"

    v = start
    while True:
        if (step > 0 and v >= end) or (step < 0 and v <= end):
            raise StopIteration
        yield v
        v += step

class ParamProcessor:
    def __init__(self, configfile, processor):
        self.configFile = configfile
        self.processor = processor

    def run(self):
        self.genOutput(self.getParamsGiven(), "")

    def getParamsGiven(self):
        paramsGiven = list()
        for line in open(self.configFile, 'r').readlines():
            line.strip(" \n")
            if len(line) <= 1 or line[0] == '#':
                continue
            paramsGiven.append(Param(line))
        return paramsGiven

    def genOutput(self, plist, soFar):
        if len(plist) == 0:
            self.processConfigFileString(soFar)
            return
        first = plist[0]
        for i in frange(first.start, first.end, first.step):
            outString = soFar + first.name + ' ' + str(i) + "\n"
            self.genOutput(plist[1:], outString)

    def processConfigFileString(self, cfgStr):
        self.processor.process(cfgStr)


class UniqueCfgFileWriter:
    def __init__(self, prefix):
        self.outputFilePrefix = outputFilePrefix

    def process(self, cfgStr):
        suffix = cfgStr.replace(" ", "_").replace("\n", "__")
        outfname = self.outputFilePrefix + "." + suffix
        open(outfname, 'w').write(cfgStr)


class ProgramTester:
    def __init__(self, program_name):
        self.program = program_name
        self.configfile = "configfiles/__fp__.config"

    def process(self, cfgStr):
        open(self.configfile, "w").write(cfgStr)
        import subprocess
        results = subprocess.Popen(self.program + " -c " + self.configfile,
                                   shell=True,
                                   stdout=subprocess.PIPE).stdout
        print "Result: %s"  % results.read().strip()


def main():
    from optparse import OptionParser
    usage="usage: %prog [options] program_to_run findparams_configfile"
    parser = OptionParser(usage=usage)
    (options, args) = parser.parse_args()

    if len(args) != 2:
        print parser.get_usage()
        return

    program = args[0]
    configfile = args[1]

    #processor = UniqueCfgFileWriter(prefix = "gen.fp")
    processor = ProgramTester(program_name = program)
    pp = ParamProcessor(configfile = "configfiles/config.findparams",
                        processor = processor)
    pp.run()


if __name__ == "__main__":
    main()

"""
format of findparams_configfile:
type, name, start_val, end_val, step
# comments
"""
