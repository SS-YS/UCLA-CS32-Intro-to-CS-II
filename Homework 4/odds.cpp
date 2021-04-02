  // Remove the odd integers from v.
  // It is acceptable if the order of the remaining even integers is not
  // the same as in the original vector.
void removeOdds(vector<int>& v)
{
    vector<int>::iterator it = v.begin();
    while (it != v.end())
    {
        if  ((*it) % 2 != 0)
            it = v.erase(it);
        else
            it++;
    }
}
