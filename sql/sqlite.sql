-- Table: Subjects
CREATE TABLE Subjects ( 
    ID          INTEGER PRIMARY KEY AUTOINCREMENT,
    Name        TEXT,
    Age         INTEGER,
    Paid        BOOLEAN,
    Gender      CHAR,
    Nonmusician BOOLEAN,
    Country     TEXT 
);


-- Table: Annotations
CREATE TABLE Annotations ( 
    ID        INTEGER PRIMARY KEY AUTOINCREMENT,
    Genre     TEXT,
    BPM       INTEGER,
    Signature TEXT 
);


-- Table: Descriptors
CREATE TABLE Descriptors ( 
    ID                 INTEGER PRIMARY KEY AUTOINCREMENT,
    Beats              BLOB,
    EventDensity       BLOB,
    BeatSalience       BLOB,
    FastMetricalLevels BLOB 
);


-- Table: Tags
CREATE TABLE Tags ( 
    ID           INTEGER PRIMARY KEY AUTOINCREMENT,
    Name         TEXT    NOT NULL,
    MinimumValue INTEGER NOT NULL,
    MaximumValue INTEGER NOT NULL,
    Description  TEXT 
);


-- Table: Stimuli
CREATE TABLE Stimuli ( 
    ID           INTEGER PRIMARY KEY AUTOINCREMENT,
    Name         TEXT,
    Path         TEXT    NOT NULL,
    Duration     INT,
    AnnotationID INTEGER REFERENCES Annotations ( ID ),
    DescriptorID INTEGER REFERENCES Descriptors ( ID ) 
);


-- Table: Experiments
CREATE TABLE Experiments ( 
    ID        INTEGER PRIMARY KEY AUTOINCREMENT,
    SubjectID INTEGER REFERENCES Subjects ( ID ),
    StimuliID INTEGER REFERENCES Stimuli ( ID ),
    Tag       INTEGER REFERENCES Tags ( ID ),
    Rating    INTEGER,
    Note      TEXT 
);



