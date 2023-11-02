# CNT4007 Project: Group 49
Final project for CNT4007 Computer Networks Fundamentals.
Project Group: 49.

Implements a variation of the BitTorrent protocol wherein peers attempt to
establish connections amongst themselves and fetch/serve pieces of some
pre-determined file. The runtime environment is configured by a common.cfg file,
whilst peer hosts/ports are specified in a PeerInfo.cfg file.

## Dependencies
1. C++ compiler (C++11 or greater).
2. [GNU Make](https://www.gnu.org/software/make/)

## Build
Run the following in a terminal, while inside the Git repository root folder:
```bash
make
```

## Usage
Run the following in a terminal, while inside the Git repository root folder:
```bash
./peerProcess [peer-id]
```
Where [peer-id] is the peer's ID (unsigned integer). For example: `./peerProcess 1001`.

## Configuration
### Example: common.cfg
```
NumberOfPreferredNeighbors 2
UnchokingInterval 5
OptimisticUnchokingInterval 15
FileName TheFile.dat
FileSize 10000232
PieceSize 32768
```

### Example: PeerInfo.cfg
```
1001 lin114-00.cise.ufl.edu 6008 1
1002 lin114-01.cise.ufl.edu 6008 0
1003 lin114-02.cise.ufl.edu 6008 0
1004 lin114-03.cise.ufl.edu 6008 0
1005 lin114-04.cise.ufl.edu 6008 0
```

Each line in PeerInfo.cfg is a separate peer specified according to the
following format:
[peer ID] [host name] [listening port] [has file or not].

## Group Members
| Name              | Email                    | Contributions |
|-------------------|--------------------------|---------------|
| Michail Zeipekki  | zeipekkim@ufl.edu        | TBD           |
| Nicholas Callahan | nicholascallahan@ufl.edu | TBD           |
| Rohith Vellore    | rvellore@ufl.edu         | TBD           |

## Submission Video
TODO
