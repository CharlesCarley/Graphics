import sys, os, subprocess


def trim(line):
    line = line.replace("\t", "")
    line = line.replace("\n", "")
    line = line.replace(" ", "")
    return line


def execString(val):
    args = val.split(" ")
    if len(args) > 0:
        try:
            subprocess.call(args)
        except:
            print("call '" + val + "' failed")
            exit(1)


def initModules():
    execString("git submodule init")
    execString("git submodule update --init --merge")


def updateModules(path):
    print("==> " + path)
    execString("git checkout master")
    execString("git pull")


def main():
    initModules()

    cur_dir = os.getcwd()

    gitmodules = os.getcwd() + os.sep + ".gitmodules"
    if not os.path.isfile(gitmodules):
        print("no .gitmodules file was found at: '%s'" % gitmodules)
        print("nothing to update")
        return

    file = open(gitmodules, mode="r")
    lines = file.readlines()

    for line in lines:
        line = trim(line)
        if line.find("path=") != -1:

            path = line.replace("path=", "")
            try:
                os.chdir(cur_dir + os.sep + path)
            except:
                print("could not change directory to: '%s'" % path)

            updateModules(path)
            os.chdir(cur_dir)


if __name__ == "__main__":
    main()
