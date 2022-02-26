# CMSC_341_Pinball_Hashing_Project
In this project, a variant of the Cuckoo hashing scheme (Pinball hashing) is implemented and its performance is tested. For more information see the project details below. In this particular case, words are placed in the hash table in a pseudo-random order according to the hashing scheme. To change which file is used as the test driver, alter the constant DRIVER in the Makefile to either smalltest, mediumtest, driver, or myDriver (in increasing order of test complexity). Functions from the standard C library are used for many functions in this project.

# Assignment Details
For this project, you will implement a variation of Cuckoo Hashing and evaluate its performance experimentally. This hashing scheme, which we will call Pinball Hashing, takes elements of Cuckoo Hashing and combines it with ideas from the study of expander graphs. The entire class will participate in this experimental evaluation. Each student will be assigned a particular version of the hashing scheme and report his/her observations. Collectively, we may be able to produce some advice on how best to implement Pinball Hashing.

Pinball Hashing is an open addressing scheme (like linear probing and quadratic probing) — that is, all of the items are stored in the array of the hash table. There are not any additional data structures — no linked lists — just an array and a size.

Pinball Hashing differs from linear and quadratic probing in that for each item there is a small number of locations where an item might be placed. (In this project, we will experiment with this small number being 5, 7 or 9.) We will call this value the degree of the Pinball hash table.

So suppose that we have a Pinball hash table with degree 5. That means there are 5 slots in the hash table where an item may be placed. Let's set aside for now how these 5 slots are determined, except to say that they can be quickly computed from the hash value. To find an item in the hash table, we simply look in all 5 slots. If we find the item, then we are done. If the item is not in any of the 5 slots, then the item is not in the hash table. In contrast, in linear probing and quadratic probing, an item might theoretically be in any slot of the hash table. So searching in a Pinball hash table is very simple.

What about insertion? Suppose we are hashing strings using a hash function h( ). Suppose that h("aardvark") = 713. We call 713 the primary slot for "aardvark". If slot 713 is available, then we put "aardvark" in that slot and we are done. If 713 is not available, we look to the other 4 auxiliary slots for "aardvark". Let's say these are slots number 973, 1516, 72 and 311. (We will consider different ways of computing the locations of the auxiliary slots. That's why we are being a bit vague about how they are determined for now.) If any of these 4 auxiliary slots are available, then we simply put "aardvark" in the available auxiliary slot. What if all of the auxiliary slots are taken? Then we randomly choose one of the auxiliary slots, eject the string in that auxiliary slot from the table and then put "aardvark" in that slot. So, let's say "bison" was ejected from slot 1516 and we put "aardvark" in slot 1516 instead. What do we do with "bison"? Well, we insert it back into the hash table and hope that one of its 4 other slots is available. What if all of the other slots for "bison" are also full? Then "bison" will eject another item, maybe "cheetah", and we hope that "cheetah" has an available slot ...

This process of ejecting items and reinserting them corresponds to a random walk in a graph. (To be technical, the vertices of the graph are the slots of the hash table and two slots/vertices are connected by an edge if one vertex is a primary slot and the other vertex is an auxiliary slot for the same item.) The theory of expander graphs says that if a good fraction of the slots in the hash table is available, then a random walk will quickly get to an available slot. What is "quickly"? and does the theory actually work? We don't know. That's why we are having you do this experiment.

There are two additional issues to consider. First, it is possible for a random walk to loop back and visit a vertex/slot that it has visited before. In the example above, perhaps re-inserting "cheetah" causes "aardvark" to be ejected and we are back where we started. Second, it is possible that the hash function maps 6 strings to the same primary slot. In that case, it would be impossible for these 6 strings to be inserted in the primary slot and 4 auxiliary slots. Every time a string gets ejected and re-inserted into the hash table, it will be hashed to the same primary slot again. Again, we are back where we started.

Both of these issues can be handled by limiting the number of ejections that are allowed during a single insert operation. If the limit is exceeded, we just throw up our hands and declare that the hash table is full. (In the implementation, you will throw an exception, not hands.) What should be the limit on the number of ejections resulting from a single insertion? is 8 good? If the limit is too low, then the random walk will not be long enough to find an available slot. If it is too high, then we may be wasting our time in the random walk without finding an available sot. Is 14 better? how about 20? We don't know. That's why we are having you do this experiment.

Note that the probability of looping back is reduced if we increase the degree. In the first case, since we pick an item among the auxiliary slots randomly, a larger degree implies a lower probability of picking an auxiliary slot that leads to looping back. A bigger degree also means more strings have to hash to the same primary slot before the hash table is declared full. On the other hand, recall that a larger degree does mean a slower search, since the search function must look in all the auxiliary slots. So, is a degree of 5 good enough? would 7 be better? or maybe even 9? or is 9 too big? We don't know. That's why we are having you do this experiment.

Finally, we still need to describe how the auxiliary slots are determined. The short answer is randomly. (The longer answer involves expander graphs and the theorem that randomly generated graphs tend to have good expansion properties.) This is somewhat counter-intuitive because we have to be able to recover the locations of the auxiliary slots every time. Otherwise search would not work. How can we find the auxiliary slots if they are "random"? We approach this in three ways: pseudo-random, a little bit random and not very random.

Option #1: pseudo-random: in this approach we store a seed for the random number generator at each slot of the hash table. Suppose that seed s is stored in slot t. Every time we work with slot t as the primary slot, we call srand(s) to set the random number generator. (We do this for the insert, delete and search functions of the hash table.)

The calls to rand() after setting this seed will always produce the same values. If the degree of the hash table is 5, then the 4 calls to rand() after setting the seed will always produce the same 4 values in the same order. We will take those 4 values to be the indices of the auxiliary slots.

Where do we get these seeds to store in the hash table? We generate them randomly, of course. (See A Note on Pseudo-Random Generators and Implementation Notes for details.)
    
Option #2: a little bit random: The first approach requires additional storage to save the random seed. In this second approach, we simply use the index of the primary slot as the random seed. The rest is the same as the first approach.
    
Option #3: not very random: Another way to avoid storing a random seed is to randomly generate a few offsets for the entire hash table. In a degree 5 hash table, we generate 4 random offsets. Suppose these offsets are 30, 72, 316 and 996. Then the auxiliary slots for primary slot 10 are 40, 82, 326 and 1006. (We just add the offsets to 10.) Similarly,the auxiliary slots for primary slot 34 are 64, 106, 350 and 1030. 

Do note that in each of these approaches, the index of the auxiliary slots have to be taken modulo the table size (which should be a prime number).

Which of these 3 approaches work better? Should we just store a random seed in each slot? use the index as the seed? or just use the same offsets for the entire table? We don't know. That's why we are having you do this experiment.
Your Assignment

Your assignment is to implement and test a version of the Pinball Hashing scheme described above. You should have received an individual email message from Prof. Chang that assigned a specific version to implement. The specification includes:

The degree of the Pinball Hash table. (Recall that the degree is the number of auxiliary slots + 1.)
The maximum number of ejections allowed when inserting a new item in the hash table.
How much randomness to use when determining the auxiliary slots (i.e., one of Option #1 pseudo-random, Option #2 a little bit random and Option #3 not very random). 
