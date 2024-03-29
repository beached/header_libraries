import lldb
import lldb.formatters.Logger


class dawVectorSynthProvider:
    def __init__(self, valobj, dict):
        self.valobj = valobj
        self.count = None
        self.start = None
        self.finish = None

    def num_children(self):
        if self.count is None:
            self.count = self.num_children_impl()
        return self.count

    def num_children_impl(self):
        try:
            start_val = self.start.GetValueAsUnsigned(0)
            finish_val = self.finish.GetValueAsUnsigned(0)

            if start_val == 0 or finish_val == 0:
                return 0
            if start_val >= finish_val:
                return 0

            num_children = (finish_val - start_val)
            if (num_children % self.data_size) != 0:
                return 0
            else:
                num_children = num_children // self.data_size
            return num_children
        except Exception as inst:
            print(type(inst))    # the exception instance
            print(inst.args)     # arguments stored in .args
            print(inst)          # __str__ allows args to be printed directly,
            return 0

    def get_child_at_index(self, index):
        logger = lldb.formatters.Logger.Logger()
        logger >> "Retrieving child " + str(index)
        if index < 0:
            return None
        if index >= self.num_children():
            return None
        try:
            offset = index * self.data_size
            return self.start.CreateChildAtOffset(
                '[' + str(index) + ']', offset, self.data_type)
        except:
            return None
    def update(self):
        self.count = None
        try:
            self.start = self.valobj.GetChildAtIndex(0)
            self.finish = self.valobj.GetChildAtIndex(1)
            self.data_type = self.start.GetType().GetPointeeType()
            self.data_size = self.data_type.GetByteSize()
#            if self.start.IsValid() and self.finish.IsValid() and self.data_type.IsValid():
#                self.count = None
#            else:
#                self.count = 0
        except Exception as inst:
            print(type(inst))    # the exception instance
            print(inst.args)     # arguments stored in .args
            print(inst)          # __str__ allows args to be printed directly,
            self.count = 0
        return False

def dawVectorSummaryProvider(valobj, dict):
    sz = valobj.EvaluateExpression("size()").GetValueAsUnsigned(0)
    #prov = dawVectorSynthProvider(valobj,None)
    #sz = prov.num_children()
    return 'size=' + str(sz)

def make_string(S, L):
    if L == 0 or S == None: return '""'
    if L > 128: L = 128
    strval = ''
    for X in range(L):
        V = S[X]
        if V == 0:
            break
        strval = strval + chr(V % 256)
    return '"' + strval + '"'

def dawStringLikeSummaryProvider(valobj, dict):
    size = valobj.EvaluateExpression("size()").GetValueAsUnsigned(0)
    if size == 0: return '""'
    data_ptr = valobj.EvaluateExpression("data()")
    if data_ptr == 0: return '<null buffer>'
    err = lldb.SBError()
    buf = valobj.process.ReadMemory(data_ptr.GetValueAsUnsigned(0),size,err)
    if err.Fail(): return '<error: %s>' % str(err)
    return 'size=' + str(size) + ': ' + make_string(buf,size)

def __lldb_init_module(debugger, dict):
    debugger.HandleCommand('type synthetic add -w daw_header_libraries_lldb -l daw_header_libraries_lldb.dawVectorSynthProvider            -x "^(daw::)vector<.+>$"')
    debugger.HandleCommand('type summary   add -w daw_header_libraries_lldb -F daw_header_libraries_lldb.dawVectorSummaryProvider       -e -x "^(daw::)vector<.+>$"')
    debugger.HandleCommand('type summary   add -w daw_header_libraries_lldb -F daw_header_libraries_lldb.dawStringLikeSummaryProvider   -e -x "^(daw::sv1::)basic_string_view<.+>$"')
    debugger.HandleCommand('type summary   add -w daw_header_libraries_lldb -F daw_header_libraries_lldb.dawStringLikeSummaryProvider   -e -x "^(daw::sv2::)basic_string_view<.+>$"')
    debugger.HandleCommand('type summary   add -w daw_header_libraries_lldb  -x "^daw::unique_ptr<.*>" --inline-children')
    debugger.HandleCommand('type summary   add -w daw_header_libraries_lldb  -x "^daw::not_null<.*>" --inline-children')
    debugger.HandleCommand('type summary   add -w daw_header_libraries_lldb  -x "^daw::wrap_iter<.*>" --inline-children')

    debugger.HandleCommand('type category enable daw_header_libraries_lldb')

