// NintendoForward.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <iomanip>
#include <bitset>
#include <deque>
#include <vector>
#include <array>
#include <math.h>

using namespace std;

int main()
{
	bool goForward = false;

	if (goForward)
	{
		int size;

		cin >> size;

		unsigned int* a = new unsigned int[size / 16]; // <- input tab to encrypt
		unsigned int* b = new unsigned int[size / 16]; // <- output tab

		for (int i = 0; i < size / 16; i++) {   // Read size / 16 integers to a
			cin >> hex >> a[i];
		}

		for (int i = 0; i < size / 16; i++) {   // Write size / 16 zeros to b
			b[i] = 0;
		}

		for (int i = 0; i < size; i++)
			for (int j = 0; j < size; j++) {
				int test = ((a[i / 32] >> (i % 32)) &
					(a[j / 32 + size / 32] >> (j % 32)) & 1) << ((i + j) % 32);
				b[(i + j) / 32] ^= test;
			}

		for (int i = 0; i < size / 16; i++) {
			if (i > 0) {
				cout << ' ';
			}
			cout << setfill('0') << setw(8) << hex << b[i];       // print result
		}
		cout << endl;
	}

	else
	{
		// Parse input
		int size;
		cin >> size;

		//ASSUME 32 BIT FOR NOW, ARRAY SIZE OF 2

		array<unsigned int, 2> outputs;

		for (int i = 0; i < 2; i++)
		{
			cin >> hex >> outputs[i];
		}

		// Translate both outputs to a bit array
		array<bitset<32>, 2> outputBits;
		for (int i = 0; i < 2; i++)
		{
			outputBits[i] = bitset<32>(outputs[i]);
		}

		// A list of potential solutions will be kept
		deque<array<vector<bool>, 2>> potentialSolutions;
		potentialSolutions.push_back({ { {},{} } });

		// Work on first output independently for now
		for (int bit = 0; bit < 32; bit++)
		{
			bool bitVal = outputBits[bit / 32].test(bit % 32);

			// Find all the i,j pairs that can affect this bit
			vector<array<int, 2>> ijPairs;
			ijPairs.clear();
			for (int i = 0; i <= bit; i++)
			{
				if (i < 32 && (bit - i) < 32)
				{
					ijPairs.push_back({ i, bit - i });

				}
			}

			size_t initialSize = potentialSolutions.size();

			// Temporarily add all four 0/1 combos to each potential solution for analysis
			// Room for optimization here for sure
			for (int i = 0; i < initialSize; i++)
			{
				// I am purposely creating a copy of the solution, not taking a reference
				array<vector<bool>, 2> solution = potentialSolutions[i];

				// I can save time by calculating all inner-flips ahead of time
				// these will be the same regardless of last move
				int innerFlips = 0;
				for (int i = 1; i < ijPairs.size() - 1; i++)
				{
					if (solution[0][ijPairs[i][0]] && solution[1][ijPairs[i][1]])
					{
						innerFlips++;
					}
				}

				int outerFlips;

				// first test the 0/0 case

				solution[0].push_back(false);
				solution[1].push_back(false);

				// Check if this solution would work
				// Count all the outer ijPairs that would lead to a flip of the bit
				// The numbers in the ij pairs should never exceed the size of the solution thus far
				outerFlips = 0;
				if (solution[0][ijPairs[0][0]] && solution[1][ijPairs[0][1]])
				{
					outerFlips++;
				}
				if (bit != 0 && solution[0][ijPairs.back()[0]] && solution[1][ijPairs.back()[1]])
				{
					outerFlips++;
				}

				// I need odd flips if the bitval is true, and vice-versa
				if ((innerFlips + outerFlips) % 2 == bitVal)
				{
					// See if I can immediately rule out this solution if possible
					

					// This is a valid potential solution
					// Create a branch
					potentialSolutions.push_back(solution);
				}

				else
				{
					// This solution is not valid
					// do not create a branch
				}

				// now test 0 / 1
				solution[1].back() = true;

				outerFlips = 0;
				if (solution[0][ijPairs[0][0]] && solution[1][ijPairs[0][1]])
				{
					outerFlips++;
				}
				if (bit != 0 && solution[0][ijPairs.back()[0]] && solution[1][ijPairs.back()[1]])
				{
					outerFlips++;
				}

				// I need odd flips if the bitval is true, and vice-versa
				if ((innerFlips + outerFlips) % 2 == bitVal)
				{
					potentialSolutions.push_back(solution);
				}

				// Don't need to test 1 / 0  since it will produce a symmetrical result, which I don't need to explicitly calculate
				// Not sure actually
				solution[1].back() = false;
				solution[0].back() = true;

				outerFlips = 0;
				if (solution[0][ijPairs[0][0]] && solution[1][ijPairs[0][1]])
				{
					outerFlips++;
				}
				if (bit != 0 && solution[0][ijPairs.back()[0]] && solution[1][ijPairs.back()[1]])
				{
					outerFlips++;
				}

				// I need odd flips if the bitval is true, and vice-versa
				if ((innerFlips + outerFlips) % 2 == bitVal)
				{
					potentialSolutions.push_back(solution);
				}

				// now test 1 / 1
				solution[1].back() = true;
				solution[0].back() = true;

				outerFlips = 0;
				if (solution[0][ijPairs[0][0]] && solution[1][ijPairs[0][1]])
				{
					outerFlips++;
				}
				if (bit != 0 && solution[0][ijPairs.back()[0]] && solution[1][ijPairs.back()[1]])
				{
					outerFlips++;
				}

				// I need odd flips if the bitval is true, and vice-versa
				if ((innerFlips + outerFlips) % 2 == bitVal)
				{
					potentialSolutions.push_back(solution);
				}
			}

			// Now that all the subsequent potential solutions have been added to the back of the queue, delete the initial ones from the front
			for (int i = 0; i < initialSize; i++)
			{
				potentialSolutions.pop_front();
			}

			cout << potentialSolutions.size() << endl;
		}


		cerr << "first output done" << endl;

		// Now work on second output
		// At this point, all my potential solutions are a pair of 32-bit numbers ready to go
		// However, this part will restrict that list
		for (int bit = 32; bit < 64; bit++)
		{
			bool bitVal = outputBits[bit / 32].test(bit % 32);

			// Find all the i,j pairs that can affect this bit
			vector<array<int, 2>> ijPairs;
			ijPairs.clear();
			for (int i = 0; i <= bit; i++)
			{
				if (i < 32 && (bit - i) < 32)
				{
					ijPairs.push_back({ i, bit - i });
				}
			}

			// Look at every solution and see if it still holds up
			// Room for optimization here for sure
			size_t initialSize = potentialSolutions.size();

			for (int i = 0; i < initialSize; i++)
			{
				array<vector<bool>, 2> & solution = potentialSolutions.front();

				// check if the solution satisifies the new requirements
				int flips = 0;
				for (auto& pair : ijPairs)
				{
					if (solution[0][pair[0]] && solution[1][pair[1]])
					{
						flips++;
					}
				}


				// I need odd flips if the bitval is true, and vice-versa
				if (flips % 2 != bitVal)
				{
					//not a valid solution anymore
					// delete it
					potentialSolutions.pop_front();
				}
				else
				{
					// still a valid solution, move to back
					potentialSolutions.push_back(potentialSolutions.front());
					potentialSolutions.pop_front();
				}
			}

			cerr << potentialSolutions.size() << endl;
		}

		// Print the solutions
		cerr << "Final hex nums" << endl;
		for (auto& solution : potentialSolutions)
		{
			int firstNum = 0;
			for (int i = 0; i < 32; i++)
			{
				firstNum += solution[0][i] * pow(2, i);
			}

			int secondNum = 0;
			for (int i = 0; i < 32; i++)
			{
				secondNum += solution[1][i] * pow(2, i);
			}

			cout << setfill('0') << setw(8) << hex << firstNum << " " << setfill('0') << setw(8) << hex << secondNum << endl;       // print result
		}
		
	}


	char exit;
	cin >> exit;
	return 0;
}



// Break it down
// First figure out exactly what happens on a single iteration of updating b
//a[i / 32] >> (i % 32) cannot be inverted easily with <<. I lose the digits that get pushed off
// The first big operation is x & y & 1, the only possible results for this are 0 and 1
// It's one if both x and y are odd
// It then gets shifted to the left by a certain number of digits. Therefore, the final result is either 0, or a single 1 in a 32 bit number
// a ^= operation will invert every bit that's a 1. So nothing will happen when it's all 0s, and a single bit will be inverted if there's a single 1.
// Therefore, b is a series of inidividual bit flips

// There can be multiple solutions to the problem, display them in ascending order

// Start with the 32-bit case
// This means that a and b are both size 2
// There is a maximum of 1024 possible bit flips, starting to seem like a discrete optimization problem
// In the first example, the 15th bit is 1 for one of the numbers
// this means that there needs to be an odd number of times where this bit was flipped
// This flip can happen when i+j % 32 == 14 (15 different ways to do this)
// This leads me to believe I should start with far right bit

// The first bit is 1 for the first number
// This flip can happen if i + j < 32 and i + j % 32 ==0 aka i and j is 0. Look at this case more closely
// For this flip to go through, the first and last bit of both inputs need to be 1

// Next step: the second bit for the first number is 1
// This flip can happen if i + j < 32 and 1 + j % 32 == 1. Acceptable pairs are 1,0 and 0,1
// I need exactly one of these flips to go through
// 1,0 will flip if second bit of first input is 1 and first bit of second input is 1
// 0,1 will flip if vice-versa
// Therefore, I need 1 second bit to be 1, and the other to be 0
// Always symmetrical solutions, so don't keep both

// I think I can just build up this way continuously, managing all possible paths

// I have done this in theory, but it takes way too long
// It can very quickly reach over 100000 paths, can't keep them all in memory
// might need a backtracking approach instead

