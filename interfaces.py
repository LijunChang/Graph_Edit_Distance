import subprocess
import os
os.chdir('/home/wg25r/pyCMT/GED/RealGED')

print(int(subprocess.check_output(
    ['./a.out', '-d', '../dataForReal/15186818.txt', '-q', '../dataForReal/15922252.txt', '-g'])) == 6)


def getGED(id1, id2):
    file1 = "../dataForReal/{}".format(id1)
    # 本来以为要先写一行行Copilot才可以，但是发现直接写Copilot执行的结果是相同的，所以直接写了
    file2 = "../dataForReal/{}".format(id2)
    return int(subprocess.check_output(['./a.out', '-d', file1, '-q', file2, '-g']))
