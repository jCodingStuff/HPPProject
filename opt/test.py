import subprocess


output_file = 'test_result.txt'
grid = ['1000', '2000', '3000', '4000', '5000', '6000', '7000']
prob = '0.5'
nsteps = '100'
debug = '0'
n_reps = 10

times = [[' ' for j in range(n_reps)] for i in grid]

for index_i, i in enumerate(grid):
    for j in range(n_reps):
        seed = str(j+1)
        command = ' '.join(['./gol', i, i, prob, nsteps, seed, debug])
        proc = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE)
        subprocess_return = proc.stdout.read().strip()
#        print(subprocess_return)
        times[index_i][j] = str(float(subprocess_return))
    print('{}% complete!'.format(((index_i+1)/len(grid))*100))

with open(output_file, 'w') as f:
    f.writelines([' '.join(line) + '\n' for line in times])
