import os
import time
import argparse
import sys

stdout = 1
stderr = 2

clear_col = "\033[0m"

lead_col = "\033[1;4;38;5;196m"
lead_no_line_col = "\033[1;0;38;5;196m"
key_col = "\033[1;0;38;5;172m"
val_col = "\033[0;38;5;215m"
light_gray = "\033[1;38;5;241m"

def exec(name, *args):
	pid = os.fork()
	if pid < 0:
		print(f"Error: {name}: {os.error.strerror}")
	elif pid == 0:
		res = os.execvp(name, (name,) + args)
		if res < 0:
			print(f"Error: {name}: {os.error.strerror}")
			os._exit(1)
	else:
		res = os.waitpid(pid, 0)

def launch(name, *args) -> float:
	start = time.time()
	exec(name, *args)
	end = time.time()

	return end - start

def dup(direction: bool):
	global stdout, stderr

	if direction:
		stdout = os.dup(stdout)
		stderr = os.dup(stderr)

		null = os.open(os.devnull, os.O_WRONLY)
		os.dup2(null, 1)
		os.dup2(null, 2)

		os.close(null)
	else:
		os.dup2(stdout, 1)
		os.dup2(stderr, 2)

		os.close(stdout)
		os.close(stderr)


def compute_stats(name, nb, *args) -> dict:
	time_min = float('inf')
	time_max = 0
	time_sum = 0

	for i in range(nb):
		percent = 100 * i / nb
		int_p = int(percent)
		if int_p == percent:
			percent = str(int_p)
		else:
			percent = f"{percent:.2f}"
		os.write(stdout, bytearray(f"Execution {i + 1}/{nb} [{percent} %]...\n", 'utf-8'))
		time_ = launch(name, *args)
		time_min = min(time_min, time_)
		time_max = max(time_max, time_)
		time_sum += time_
		os.write(stdout, bytearray(f"{lead_col}Current values{lead_no_line_col}: {key_col}Min{val_col}: {time_min:.2f}s{clear_col}, {key_col}Max{val_col}: {time_max:.2f}s{clear_col}, {key_col}Avg{val_col}: {time_sum / (i + 1):.2f}s{clear_col}\n", 'utf-8'))
	return { 'Min': time_min, 'Max': time_max, 'Average': time_sum / nb }

def test(*args, **kwargs):
	nb = kwargs.get('nb', 1)
	mine = "./ft_ls"
	true = "ls"

	dup(True)

	os.write(stdout, bytearray("Testing the true ls...\n", 'utf-8'))
	true_dict = compute_stats(true, nb, *args)
	os.write(stdout, bytearray("Testing my ls...\n", 'utf-8'))
	mine_dict = compute_stats(mine, nb, *args)

	dup(False)

	return { 'mine': mine_dict, 'true': true_dict }


def print_values(leading, values):
	first = True

	print(f"{lead_col}{leading}{lead_no_line_col}: ", end="")
	for (key, value) in values.items():
		if first:
			first = False
		else:
			print(" ", end="")
		print(f"{key_col}{key} {val_col}{value:.4f}s", end="")

	print(f"{clear_col}")


def main(ac, av) -> int:
	if ac == 1:
		args = ["."]
	else:
		args = av[1:]

	opts = None

	parser = argparse.ArgumentParser(description='Test ft_ls')
	parser.add_argument('-n', '--nb', metavar='number of tests', dest='tests', type=int, default=10, help='Number of times to run the test')
	parser.add_argument('args', nargs='*', default=['-l', '.'], help='Arguments to pass to ls and ft_ls')

	opts = parser.parse_args(args)

	args = opts.args
	n = opts.tests

	print(f"Will run {n} tests on `{light_gray}{' '.join(args)}{clear_col}` with both ft_ls and ls")
	print("")

	res = test(*args, nb=n)

	print_values("True", res["true"])
	print_values("Mine", res["mine"])

	ratio = res['mine']['Average'] / res['true']['Average']
	print(f"{lead_col}Ratio{lead_no_line_col}: {clear_col}{light_gray}{ratio:.4f}{clear_col}")

	return 0

if __name__ == '__main__':
	ret = main(len(sys.argv), sys.argv)
	sys.exit(ret)
