# CNT4007 Project: Group 49
Final project for CNT4007 Computer Networks Fundamentals.
Project Group: 49.

Implements a variation of the BitTorrent protocol wherein peers attempt to
establish connections amongst themselves and fetch/serve pieces of some
pre-determined file. The runtime environment is configured by a Common.cfg file,
whilst peer hosts/ports are specified in a PeerInfo.cfg file.

## Dependencies
1. C++ compiler (C++20 or greater).
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
The project expects two configuration files in its current working directory: Common.cfg and PeerInfo.cfg. Additionally, read the PeerInfo.cfg example section for instructions on where to place the file being shared.

### Example: Common.cfg
```
NumberOfPreferredNeighbors 2
UnchokingInterval 5
OptimisticUnchokingInterval 15
FileName TheFile.dat
FileSize 10000232
PieceSize 32768
```

The configuration attributes must appear in the order shown above. Lines are separated by the new line character (i.e. \n, no carriage returns, etc.).

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

Each file must be separated by a newline character (i.e \n, no carriage returns, etc). Similarly, there should be no trailing spaces or lines.

The peers that own the file must have the file in their respective directories. For instance, if peer 1001 has the file, then the file must be located in peer_1001/fileName.ext. All paths are relative to the current working directory.

## Group Members
| Name              | Email                    | Contributions |
|-------------------|--------------------------|---------------|
| Michail Zeipekki  | zeipekkim@ufl.edu        | PeerController & PeerProcess (server, discovery, downloader routine) |
| Nicholas Callahan | nicholascallahan@ufl.edu | PeerProcess (optimistic and preferred peer selection)           |
| Rohith Vellore    | rvellore@ufl.edu         | Logging & Testing           |

## Submission Video

https://1drv.ms/v/s!AirWD0kykA32iqkjGk5IATl6eRL1Fw?e=Ad4ypm
