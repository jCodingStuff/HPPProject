import subprocess


output_file = 'test_result.txt'
grid = '7000'
prob = '0.5'
nsteps = '100'
debug = '0'
min_threads, max_threads = 1, 25
n_threads = range(min_threads, max_threads + 1)
n_reps = 10

times = [[' ' for j in range(n_reps)] for i in n_threads]

for index_i, i in enumerate(n_threads):
    for j in range(n_reps):
        seed = str(j+1)
        command = ' '.join(['./gol', grid, grid, prob, nsteps, seed, str(i), debug])
        proc = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE)
        subprocess_return = proc.stdout.read().strip()
#        print(subprocess_return)
        times[index_i][j] = str(float(subprocess_return))
    print('{}% complete!'.format(((index_i+1)/len(n_threads))*100))

with open(output_file, 'w') as f:
    f.writelines([' '.join(line) + '\n' for line in times])
