import re
import logging


class daw_vector_printer:
    def __init__(self, val):
        self.val = val

    def to_string(self):
        first = self.val['m_begin']
        last = self.val['m_end']
        size = 0
        if first != 0 and last != 0:
            size = int(last - first)
        return 'size: %d' % (size)

    def children(self):
        first = self.val['m_begin']
        last = self.val['m_end']
        size = 0
        if first != 0 and last != 0:
            size = last - first
        if size == 0:
            return
        for i in range(size):
            yield ('[{}]'.format(i), first.dereference())
            first = first + 1

    def display_hint(self):
        return 'array'


def lookup_daw_vector(val):
    lookup_tag = val.type.tag
    if lookup_tag is None:
        return None
    regex = re.compile("^daw::vector<.*>$")
    if regex.match(lookup_tag):
        return daw_vector_printer(val)
    return None


class daw_string_view_printer:
    def __init__(self, val):
        self.val = val

    def to_string(self):
        m_first = self.val['m_first']
        return m_first.string()

    def children(self):
        m_first = self.val['m_first']
        m_last = self.val['m_last']
        result = []
        result.append(('m_first', m_first))
        result.append(('m_last', m_last))
        return result

    def display_hint(self):
        return 'string'


def lookup_daw_string_view(val):
    lookup_tag = val.type.tag
    if lookup_tag is None:
        return None
    regex = re.compile("^daw::sv2::basic_string_view<.*>$")
    regex2 = re.compile("^daw::sv2::string_view<.*>$")
    regex3 = re.compile("^daw::sv2::string_view$")
    if regex.match(lookup_tag) or regex2.match( lookup_tag ) or regex3.match( lookup_tag ):
        return daw_string_view_printer(val)
    return None


gdb.pretty_printers.append(lookup_daw_vector)
gdb.pretty_printers.append(lookup_daw_string_view)
