-- Table: Subjects
CREATE TABLE Subjects ( 
    ID          INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
    Name        TEXT,
    Age         INTEGER,
    Paid        BOOLEAN,
    Gender      CHAR,
    Nonmusician BOOLEAN,
    Country     TEXT 
);

-- Table: Annotations
CREATE TABLE Annotations ( 
    ID        INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
    Genre     TEXT,
    BPM       INTEGER,
    Signature TEXT 
);

-- Table: Descriptors
CREATE TABLE Descriptors ( 
    ID                 INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
    Beats              BLOB,
    EventDensity       BLOB,
    BeatSalience       BLOB,
    FastMetricalLevels BLOB 
);

-- Table: Tags
CREATE TABLE Tags ( 
    ID           INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
    Name         TEXT    NOT NULL,
    MinimumValue INTEGER NOT NULL,
    MaximumValue INTEGER NOT NULL,
    Description  TEXT 
);

-- Table: Stimuli
CREATE TABLE Stimuli ( 
    ID           INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
    Name         TEXT,
    Path         TEXT    NOT NULL,
    Duration     INT,
    Tagged       BOOLEAN DEFAULT (0),
    AnnotationID INTEGER REFERENCES Annotations ( ID ),
    DescriptorID INTEGER REFERENCES Descriptors ( ID )
);

-- Table: Experiments
CREATE TABLE Experiments ( 
    ID        INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
    SubjectID INTEGER REFERENCES Subjects ( ID ),
    StimuliID INTEGER REFERENCES Stimuli ( ID ),
    Tag       INTEGER REFERENCES Tags ( ID ),
    Rating    INTEGER,
    Note      TEXT 
);

-- Table: Metada
CREATE TABLE Metadata (
    ID      INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
    Version REAL NOT NULL
);

-- Version
INSERT INTO "Metadata" VALUES(1, 0.0.1);
