import pandas

class UDI_reader():
    def __init__(self):
        self.UDI_file_path = "/media/mnt/COVID-19/Central_data_folder/UDI_list.xlsx"
        self.read_UDI()

    def read_UDI(self):
        self.db = pandas.read_excel(self.UDI_file_path, "UDI_1")
        self.db = self.db.append(pandas.read_excel(self.UDI_file_path, "UDI_2"), ignore_index=True)
        self.db = self.db.append(pandas.read_excel(self.UDI_file_path, "JSI_irrad_Aug2020"), ignore_index=True)

    def print_UDI_list(self):
        print (self.db)

    def get_index(self, UDI_n):
        UDI_n = str(int(UDI_n))
        return self.db.index[self.db.UDI == UDI_n]

    def get_pin_charge(self, UDI_n):
        ix = self.get_index(UDI_n)
        return float(self.db["pin_c"][ix])

    def get_foot(self, UDI_n):
        ix = self.get_index(UDI_n)
        return float(self.db["foot_cv"][ix])

    def get_fluence(self, UDI_n):
        ix = self.get_index(UDI_n)
        return [self.db["fluence_t"][ix], float(self.db["fluence"][ix])]

    def get_name(self, UDI_n):
        ix = self.get_index(UDI_n)
        return self.db.iloc[ix]["name"]

if __name__ == '__main__':
    reader = UDI_reader()
    #reader.print_UDI_list()

    check_UDI = 210

    ix = reader.get_index(check_UDI)
    print (ix)

    name = reader.get_name(check_UDI)
    fluence = reader.get_fluence(check_UDI)
    pinc = reader.get_pin_charge(check_UDI)
    foot = reader.get_foot(check_UDI)
    print (name, fluence)
    print (f"Params: {foot} {pinc}")
